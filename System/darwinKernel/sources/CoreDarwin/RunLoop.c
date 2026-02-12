/*
 * CoreDarwin - RunLoop Implementation
 * 
 * CFRunLoop implementation for PocketDarwin launchd
 */

#include "CoreDarwin.h"
#include <sys/event.h>
#include <pthread.h>
#include <unistd.h.h>
#include <time.h>

/* RunLoop Implementation */
typedef struct _CFRunLoop {
    CFObject base;
    int kqueue_fd;
    pthread_t thread;
    bool running;
    CFRunLoopTimerRef timers;
    CFRunLoopSourceRef sources;
    CFRunLoopObserverRef observers;
} CFRunLoop;

/* RunLoop Timer Implementation */
typedef struct _CFRunLoopTimer {
    CFObject base;
    CFRunLoopRef runLoop;
    CFTimeInterval interval;
    CFTimeInterval nextFire;
    bool repeats;
    CFRunLoopTimerCallBack callback;
    void *context;
    struct _CFRunLoopTimer *next;
} CFRunLoopTimer;

/* RunLoop Source Implementation */
typedef struct _CFRunLoopSource {
    CFObject base;
    CFRunLoopRef runLoop;
    CFRunLoopSourceCallBacks callbacks;
    void *context;
    int fd;
    struct _CFRunLoopSource *next;
} CFRunLoopSource;

/* RunLoop Observer Implementation */
typedef struct _CFRunLoopObserver {
    CFObject base;
    CFRunLoopRef runLoop;
    CFOptionFlags activities;
    CFRunLoopObserverCallBack callback;
    void *context;
    struct _CFRunLoopObserver *next;
} CFRunLoopObserver;

/* Global RunLoops */
static CFRunLoopRef mainRunLoop = NULL;
static __thread CFRunLoopRef currentRunLoop = NULL;

/* RunLoop Creation and Management */
CFRunLoopRef CFRunLoopGetCurrent(void) {
    if (!currentRunLoop) {
        CFRunLoop *rl = malloc(sizeof(CFRunLoop));
        if (rl) {
            rl->base.base._cfTypeID = kCFRunLoopTypeID;
            rl->base.retainCount = 1;
            pthread_mutex_init(&rl->base.lock, NULL);
            
            rl->kqueue_fd = kqueue();
            rl->running = false;
            rl->timers = NULL;
            rl->sources = NULL;
            rl->observers = NULL;
            
            currentRunLoop = (CFRunLoopRef)rl;
        }
    }
    return currentRunLoop;
}

CFRunLoopRef CFRunLoopGetMain(void) {
    if (!mainRunLoop) {
        mainRunLoop = CFRunLoopGetCurrent();
    }
    return mainRunLoop;
}

/* Timer Management */
CFRunLoopTimerRef CFRunLoopTimerCreate(CFAllocatorRef allocator, CFAbsoluteTime fireDate, CFTimeInterval interval, CFOptionFlags flags, CFIndex order, CFRunLoopTimerCallBack callout, CFRunLoopTimerContext *context) {
    CFRunLoopTimer *timer = malloc(sizeof(CFRunLoopTimer));
    if (!timer) return NULL;
    
    timer->base.base._cfTypeID = kCFRunLoopTypeID;
    timer->base.retainCount = 1;
    pthread_mutex_init(&timer->base.lock, NULL);
    
    timer->interval = interval;
    timer->nextFire = fireDate;
    timer->repeats = (interval > 0);
    timer->callback = callout;
    timer->context = context ? context->info : NULL;
    timer->next = NULL;
    
    return (CFRunLoopTimerRef)timer;
}

void CFRunLoopAddTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFRunLoopMode mode) {
    if (!rl || !timer) return;
    
    CFRunLoop *runLoop = (CFRunLoop *)rl;
    CFRunLoopTimer *t = (CFRunLoopTimer *)timer;
    
    t->runLoop = rl;
    
    // Add to timer list
    if (!runLoop->timers) {
        runLoop->timers = t;
    } else {
        CFRunLoopTimer *current = runLoop->timers;
        while (current->next) {
            current = current->next;
        }
        current->next = t;
    }
}

/* Source Management */
CFRunLoopSourceRef CFRunLoopSourceCreate(CFAllocatorRef allocator, CFIndex order, CFRunLoopSourceCallBacks *callBacks) {
    CFRunLoopSource *source = malloc(sizeof(CFRunLoopSource));
    if (!source) return NULL;
    
    source->base.base._cfTypeID = kCFRunLoopTypeID;
    source->base.retainCount = 1;
    pthread_mutex_init(&source->base.lock, NULL);
    
    source->callbacks = *callBacks;
    source->context = callBacks ? callBacks->info : NULL;
    source->fd = -1;
    source->next = NULL;
    
    return (CFRunLoopSourceRef)source;
}

void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFRunLoopMode mode) {
    if (!rl || !source) return;
    
    CFRunLoop *runLoop = (CFRunLoop *)rl;
    CFRunLoopSource *s = (CFRunLoopSource *)source;
    
    s->runLoop = rl;
    
    // Add to source list
    if (!runLoop->sources) {
        runLoop->sources = s;
    } else {
        CFRunLoopSource *current = runLoop->sources;
        while (current->next) {
            current = current->next;
        }
        current->next = s;
    }
    
    // Add to kqueue if source has a file descriptor
    if (s->fd >= 0) {
        struct kevent kev;
        EV_SET(&kev, s->fd, EVFILT_READ, EV_ADD, 0, 0, s);
        kevent(runLoop->kqueue_fd, &kev, 1, NULL, 0, NULL);
    }
}

/* Observer Management */
CFRunLoopObserverRef CFRunLoopObserverCreate(CFAllocatorRef allocator, CFOptionFlags activities, Boolean repeats, CFIndex order, CFRunLoopObserverCallBack callout, CFRunLoopObserverContext *context) {
    CFRunLoopObserver *observer = malloc(sizeof(CFRunLoopObserver));
    if (!observer) return NULL;
    
    observer->base.base._cfTypeID = kCFRunLoopTypeID;
    observer->base.retainCount = 1;
    pthread_mutex_init(&observer->base.lock, NULL);
    
    observer->activities = activities;
    observer->callback = callout;
    observer->context = context ? context->info : NULL;
    observer->next = NULL;
    
    return (CFRunLoopObserverRef)observer;
}

void CFRunLoopAddObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFRunLoopMode mode) {
    if (!rl || !observer) return;
    
    CFRunLoop *runLoop = (CFRunLoop *)rl;
    CFRunLoopObserver *o = (CFRunLoopObserver *)observer;
    
    o->runLoop = rl;
    
    // Add to observer list
    if (!runLoop->observers) {
        runLoop->observers = o;
    } else {
        CFRunLoopObserver *current = runLoop->observers;
        while (current->next) {
            current = current->next;
        }
        current->next = o;
    }
}

/* RunLoop Execution */
static void *runLoopThread(void *arg) {
    CFRunLoop *runLoop = (CFRunLoop *)arg;
    runLoop->running = true;
    
    while (runLoop->running) {
        struct kevent events[16];
        struct timespec timeout;
        CFAbsoluteTime now = CFAbsoluteTimeGetCurrent();
        CFAbsoluteTime nextTimer = now + 1.0; // Default 1 second timeout
        
        // Find next timer fire time
        CFRunLoopTimer *timer = runLoop->timers;
        while (timer) {
            if (timer->nextFire < nextTimer) {
                nextTimer = timer->nextFire;
            }
            timer = timer->next;
        }
        
        // Set timeout
        timeout.tv_sec = (time_t)(nextTimer - now);
        timeout.tv_nsec = (long)((nextTimer - now - timeout.tv_sec) * 1000000000);
        
        // Wait for events
        int nev = kevent(runLoop->kqueue_fd, NULL, 0, events, 16, &timeout);
        
        if (nev > 0) {
            for (int i = 0; i < nev; i++) {
                struct kevent *kev = &events[i];
                
                if (kev->filter == EVFILT_READ) {
                    // Handle source events
                    CFRunLoopSource *source = (CFRunLoopSource *)kev->udata;
                    if (source && source->callbacks.perform) {
                        source->callbacks.perform(source, source->context);
                    }
                }
            }
        }
        
        // Check and fire timers
        now = CFAbsoluteTimeGetCurrent();
        timer = runLoop->timers;
        while (timer) {
            if (timer->nextFire <= now) {
                if (timer->callback) {
                    timer->callback(timer, timer->context);
                }
                
                if (timer->repeats) {
                    timer->nextFire = now + timer->interval;
                }
            }
            timer = timer->next;
        }
        
        // Notify observers
        CFRunLoopObserver *observer = runLoop->observers;
        while (observer) {
            if (observer->callback) {
                observer->callback(observer, kCFRunLoopAllActivities, observer->context);
            }
            observer = observer->next;
        }
    }
    
    return NULL;
}

void CFRunLoopRun(void) {
    CFRunLoopRef rl = CFRunLoopGetCurrent();
    if (!rl) return;
    
    CFRunLoop *runLoop = (CFRunLoop *)rl;
    
    if (!runLoop->running) {
        pthread_create(&runLoop->thread, NULL, runLoopThread, runLoop);
        pthread_join(runLoop->thread, NULL);
    }
}

void CFRunLoopStop(CFRunLoopRef rl) {
    if (!rl) return;
    
    CFRunLoop *runLoop = (CFRunLoop *)rl;
    runLoop->running = false;
    
    if (runLoop->kqueue_fd >= 0) {
        close(runLoop->kqueue_fd);
        runLoop->kqueue_fd = -1;
    }
}

/* Utility Functions */
CFAbsoluteTime CFAbsoluteTimeGetCurrent(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (CFAbsoluteTime)ts.tv_sec + (CFAbsoluteTime)ts.tv_nsec / 1000000000.0;
}
