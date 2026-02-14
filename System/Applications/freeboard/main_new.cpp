/*
 * FreeBoard - Modern OpenGL-based launcher for PocketDarwin
 * Inspired by iOS dock design with wallpaper, status bar, and blurred dock
 * Usage: freeboard [--resolution WIDTHxHEIGHT] [--wallpaper PATH]
 */

#include "FreeBoardUI.h"
#include <iostream>
#include <getopt.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <csignal>
#include <atomic>

std::atomic<bool> g_running(true);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        g_running = false;
    }
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -r, --resolution WIDTHxHEIGHT  Set window resolution (default: 800x600)\n";
    std::cout << "  -w, --wallpaper PATH          Set wallpaper image path\n";
    std::cout << "  -h, --help                     Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " -r 1024x768\n";
    std::cout << "  " << programName << " -r 1280x720 -w /path/to/wallpaper.png\n";
    std::cout << "  " << programName << " --resolution 1920x1080 --wallpaper ~/Pictures/bg.jpg\n";
}

bool parseArguments(int argc, char* argv[], FreeBoardUI::Resolution& resolution, std::string& wallpaperPath) {
    static struct option long_options[] = {
        {"resolution", required_argument, 0, 'r'},
        {"wallpaper",  required_argument, 0, 'w'},
        {"help",       no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "r:w:h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'r':
                resolution = FreeBoardUI::parseResolution(optarg);
                if (resolution.width <= 0 || resolution.height <= 0) {
                    std::cerr << "Error: Invalid resolution format. Use WIDTHxHEIGHT (e.g., 1024x768)\n";
                    return false;
                }
                break;
                
            case 'w':
                wallpaperPath = optarg;
                break;
                
            case 'h':
                printUsage(argv[0]);
                return false;
                
            case '?':
                std::cerr << "Use -h or --help for usage information.\n";
                return false;
                
            default:
                break;
        }
    }
    
    return true;
}

bool validateEnvironment() {
    // Check if X11 display is available
    Display* testDisplay = XOpenDisplay(nullptr);
    if (!testDisplay) {
        std::cerr << "Error: Cannot connect to X11 display. Make sure X11 is running.\n";
        return false;
    }
    XCloseDisplay(testDisplay);
    
    // Check if OpenGL is available
    // This is a basic check - more comprehensive checks would require GLX
    return true;
}

void printSystemInfo(const FreeBoardUI::Resolution& resolution) {
    std::cout << "FreeBoard - PocketDarwin Modern Launcher\n";
    std::cout << "========================================\n";
    std::cout << "Resolution: " << resolution.width << "x" << resolution.height << "\n";
    std::cout << "OpenGL Renderer: Enabled\n";
    std::cout << "UI Style: Modern with blur effects\n";
    std::cout << "Press ESC or Ctrl+C to exit\n\n";
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Default settings
    FreeBoardUI::Resolution resolution(800, 600);
    std::string wallpaperPath;
    
    // Parse command line arguments
    if (!parseArguments(argc, argv, resolution, wallpaperPath)) {
        return 1;
    }
    
    // Validate environment
    if (!validateEnvironment()) {
        return 1;
    }
    
    // Print system information
    printSystemInfo(resolution);
    
    // Initialize FreeBoard UI
    ::FreeBoardUI::FreeBoardUI ui;
    if (!ui.initialize(resolution)) {
        std::cerr << "Error: Failed to initialize FreeBoard UI\n";
        return 1;
    }
    
    // Load wallpaper if specified
    if (!wallpaperPath.empty()) {
        if (!ui.loadWallpaper(wallpaperPath)) {
            std::cerr << "Warning: Failed to load wallpaper from '" << wallpaperPath << "'\n";
            std::cerr << "Using gradient fallback wallpaper\n";
        } else {
            std::cout << "Loaded wallpaper: " << wallpaperPath << "\n";
        }
    }
    
    std::cout << "FreeBoard started successfully!\n";
    
    // Main event loop
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Error: Cannot open X display for event handling\n";
        return 1;
    }
    
    // Create a simple window for event handling (the UI creates its own OpenGL window)
    Window root = DefaultRootWindow(display);
    XSelectInput(display, root, KeyPressMask | KeyReleaseMask);
    
    // Time tracking for status bar updates
    auto lastTimeUpdate = std::chrono::steady_clock::now();
    const auto timeUpdateInterval = std::chrono::seconds(1);
    
    while (g_running) {
        // Handle X11 events
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            
            switch (event.type) {
                case KeyPress:
                    {
                        KeySym keysym = XLookupKeysym(&event.xkey, 0);
                        if (keysym == XK_Escape) {
                            std::cout << "ESC pressed - exiting...\n";
                            g_running = false;
                        } else if (keysym == XK_q || keysym == XK_Q) {
                            std::cout << "Q pressed - exiting...\n";
                            g_running = false;
                        }
                    }
                    break;
                    
                case KeyRelease:
                    // Handle key release if needed
                    break;
                    
                default:
                    break;
            }
        }
        
        // Update status bar time
        auto now = std::chrono::steady_clock::now();
        if (now - lastTimeUpdate >= timeUpdateInterval) {
            if (auto statusBar = ui.getStatusBar()) {
                statusBar->updateTime();
            }
            lastTimeUpdate = now;
        }
        
        // Render frame
        ui.render();
        
        // Small delay to prevent excessive CPU usage
        usleep(16000); // ~60 FPS
    }
    
    std::cout << "Shutting down FreeBoard...\n";
    
    // Cleanup
    ui.cleanup();
    XCloseDisplay(display);
    
    std::cout << "FreeBoard exited cleanly.\n";
    return 0;
}
