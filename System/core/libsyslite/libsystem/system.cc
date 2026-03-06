#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

extern "C" {

void *mach_init_routine = nullptr;
void *_cthread_init_routine = nullptr;

void __keymgr_dwarf2_register_sections(void) {}

char ***_NSGetEnviron(void) { return &environ; }

pid_t __darwin_getpid(void) { return (pid_t)syscall(SYS_getpid); }
pid_t __darwin_getppid(void) { return (pid_t)syscall(SYS_getppid); }
unsigned int __darwin_sleep(unsigned int seconds) {
  struct timespec req;
  req.tv_sec = (time_t)seconds;
  req.tv_nsec = 0;
  while (nanosleep(&req, &req) == -1 && errno == EINTR) {
  }
  return 0;
}
int __darwin_usleep(useconds_t usec) {
  struct timespec req;
  req.tv_sec = (time_t)(usec / 1000000);
  req.tv_nsec = (long)(usec % 1000000) * 1000L;
  return nanosleep(&req, nullptr);
}
[[noreturn]] void __darwin_abort(void) {
  raise(SIGABRT);
  syscall(SYS_exit_group, 134);
  __builtin_unreachable();
}
[[noreturn]] void __darwin_exit(int status) {
  syscall(SYS_exit_group, status);
  __builtin_unreachable();
}

// File descriptor I/O
int __darwin_open(const char *path, int flags, mode_t mode) {
  return (int)syscall(SYS_openat, AT_FDCWD, path, flags, mode);
}
int __darwin_close(int fd) { return (int)syscall(SYS_close, fd); }
ssize_t __darwin_read(int fd, void *buf, size_t count) {
  return (ssize_t)syscall(SYS_read, fd, buf, count);
}
ssize_t __darwin_write(int fd, const void *buf, size_t count) {
  return (ssize_t)syscall(SYS_write, fd, buf, count);
}
off_t __darwin_lseek(int fd, off_t offset, int whence) {
  return (off_t)syscall(SYS_lseek, fd, offset, whence);
}
int __darwin_fsync(int fd) { return (int)syscall(SYS_fsync, fd); }

int __darwin_unlink(const char *path) {
  return (int)syscall(SYS_unlinkat, AT_FDCWD, path, 0);
}
int __darwin_mkdir(const char *path, mode_t mode) {
  return (int)syscall(SYS_mkdirat, AT_FDCWD, path, mode);
}
int __darwin_rmdir(const char *path) {
  return (int)syscall(SYS_unlinkat, AT_FDCWD, path, AT_REMOVEDIR);
}
int __darwin_stat(const char *path, struct stat *st) { return stat(path, st); }
int __darwin_lstat(const char *path, struct stat *st) {
  return lstat(path, st);
}
int __darwin_fstat(int fd, struct stat *st) { return fstat(fd, st); }

void *__darwin_malloc(size_t size) { return malloc(size); }
void *__darwin_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void *__darwin_realloc(void *ptr, size_t size) { return realloc(ptr, size); }
void __darwin_free(void *ptr) { free(ptr); }
char *__darwin_strdup(const char *s) { return strdup(s); }
size_t __darwin_strlen(const char *s) { return strlen(s); }
int __darwin_strcmp(const char *a, const char *b) { return strcmp(a, b); }
int __darwin_strncmp(const char *a, const char *b, size_t n) {
  return strncmp(a, b, n);
}
void *__darwin_memcpy(void *dst, const void *src, size_t n) {
  return memcpy(dst, src, n);
}
void *__darwin_memset(void *s, int c, size_t n) { return memset(s, c, n); }
char *__darwin_getenv(const char *name) { return getenv(name); }
int __darwin_setenv(const char *name, const char *value, int overwrite) {
  return setenv(name, value, overwrite);
}
int __darwin_unsetenv(const char *name) { return unsetenv(name); }
time_t __darwin_time(time_t *tloc) { return time(tloc); }
int __darwin_clock_gettime(clockid_t clk_id, struct timespec *tp) {
  return (int)syscall(SYS_clock_gettime, clk_id, tp);
}
unsigned int arc4random(void) {
  static bool seeded = false;
  if (!seeded) {
    srand((unsigned int)time(nullptr));
    seeded = true;
  }
  return (unsigned int)rand();
}
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }
  return __darwin_open(path, flags, mode);
}
int close(int fd) { return __darwin_close(fd); }
ssize_t read(int fd, void *buf, size_t count) {
  return __darwin_read(fd, buf, count);
}
ssize_t write(int fd, const void *buf, size_t count) {
  return __darwin_write(fd, buf, count);
}
off_t lseek(int fd, off_t offset, int whence) {
  return __darwin_lseek(fd, offset, whence);
}
int fsync(int fd) { return __darwin_fsync(fd); }
int unlink(const char *path) { return __darwin_unlink(path); }
int mkdir(const char *path, mode_t mode) { return __darwin_mkdir(path, mode); }
int rmdir(const char *path) { return __darwin_rmdir(path); }
pid_t getpid(void) { return __darwin_getpid(); }
pid_t getppid(void) { return __darwin_getppid(); }
unsigned int sleep(unsigned int seconds) { return __darwin_sleep(seconds); }
int usleep(useconds_t usec) { return __darwin_usleep(usec); }
[[noreturn]] void abort(void) { __darwin_abort(); }
[[noreturn]] void _exit(int status) { __darwin_exit(status); }
int clock_gettime(clockid_t clk_id, struct timespec *tp) {
  return __darwin_clock_gettime(clk_id, tp);
}
}
