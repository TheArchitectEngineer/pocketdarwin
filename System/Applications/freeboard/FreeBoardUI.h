/*
 * FreeBoard UI - Modern OpenGL-based layout system
 * Inspired by iOS dock design with wallpaper, status bar, and blurred dock
 */

#ifndef FREEBOARD_UI_H
#define FREEBOARD_UI_H

#define GL_SILENCE_DEPRECATION

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#endif
#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace FreeBoardUI {

struct Resolution {
    int width = 800;
    int height = 600;
    
    Resolution() = default;
    Resolution(int w, int h) : width(w), height(h) {}
};

struct LayoutConstraints {
    // Status bar
    int statusBarHeight = 40;
    
    // Dock
    int dockHeight = 120;
    int dockCornerRadius = 20;
    float dockOpacity = 0.7f;
    int dockPaddingX = 40;
    int dockIconSpacing = 12;
    
    // Icons
    int iconSize = 60;
    int iconCornerRadius = 12;
    
    // General spacing
    int screenPadding = 20;
};

class WallpaperRenderer {
public:
    WallpaperRenderer();
    ~WallpaperRenderer();
    
    bool loadWallpaper(const std::string& imagePath);
    void render(const Resolution& resolution);
    void bindTexture();
    void unbindTexture();
    GLuint getTextureID() const { return textureID_; }
    
private:
    GLuint textureID_;
    bool loaded_;
    int imageWidth_, imageHeight_;
};

class BlurShader {
public:
    BlurShader();
    ~BlurShader();
    
    bool initialize();
    void cleanup();
    
    void applyBlur(GLuint sourceTexture, GLuint targetTexture, 
                   int width, int height, float blurRadius);
    
private:
    GLuint program_;
    GLuint vertexShader_;
    GLuint fragmentShader_;
    GLuint framebuffer_;
    GLuint quadVBO_;
    
    const char* vertexShaderSource_;
    const char* fragmentShaderSource_;
    
    GLuint compileShader(GLenum type, const char* source);
    GLuint linkProgram(GLuint vertex, GLuint fragment);
};

class StatusBar {
public:
    StatusBar(const LayoutConstraints& constraints);
    
    void render(const Resolution& resolution);
    void updateTime();
    
private:
    LayoutConstraints constraints_;
    std::string currentTime_;
    int batteryPercentage_;
    bool wifiConnected_;
    
    void renderTime(const Resolution& resolution);
    void renderBattery(const Resolution& resolution);
    void renderWiFi(const Resolution& resolution);
};

class DockIcon {
public:
    DockIcon(const std::string& name, const std::string& color = "#4A90E2");
    
    void render(int x, int y, int size, int cornerRadius);
    const std::string& getName() const { return name_; }
    
private:
    std::string name_;
    std::string color_;
    char initial_;
};

class Dock {
public:
    Dock(const LayoutConstraints& constraints, std::shared_ptr<BlurShader> blurShader);
    
    void addIcon(const std::shared_ptr<DockIcon>& icon);
    void render(const Resolution& resolution, GLuint wallpaperTexture);
    
    int getDockY(const Resolution& resolution) const;
    int getDockWidth(const Resolution& resolution) const;
    int getDockX(const Resolution& resolution) const;
    
private:
    LayoutConstraints constraints_;
    std::shared_ptr<BlurShader> blurShader_;
    std::vector<std::shared_ptr<DockIcon>> icons_;
    
    void renderBackground(const Resolution& resolution, GLuint wallpaperTexture);
    void renderIcons(const Resolution& resolution);
};

class FreeBoardUI {
public:
    FreeBoardUI();
    ~FreeBoardUI();
    
    bool initialize(const Resolution& resolution);
    void cleanup();
    
    void render();
    void handleResize(int width, int height);
    
    bool loadWallpaper(const std::string& imagePath);
    
    // Layout access
    std::shared_ptr<Dock> getDock() { return dock_; }
    std::shared_ptr<StatusBar> getStatusBar() { return statusBar_; }
    
private:
    Resolution resolution_;
    LayoutConstraints constraints_;
    
    Display* display_;
    Window window_;
#ifdef __APPLE__
    void* glContext_;  // Generic pointer for macOS
#else
    GLXContext glContext_;
#endif
    
    std::shared_ptr<WallpaperRenderer> wallpaper_;
    std::shared_ptr<BlurShader> blurShader_;
    std::shared_ptr<StatusBar> statusBar_;
    std::shared_ptr<Dock> dock_;
    
    bool initializeOpenGL();
    void createDefaultIcons();
    bool renderFallback();
};

// Utility functions
Resolution parseResolution(const std::string& resString);
std::string getCurrentTimeString();
int getBatteryPercentage();
bool isWiFiConnected();

} // namespace FreeBoardUI

#endif // FREEBOARD_UI_H
