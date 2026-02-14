/*
 * FreeBoard UI Implementation - Modern OpenGL-based layout system
 */

#include "FreeBoardUI.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace FreeBoardUI {

// Utility Functions
Resolution parseResolution(const std::string& resString) {
    Resolution res;
    size_t xPos = resString.find('x');
    if (xPos != std::string::npos) {
        res.width = std::stoi(resString.substr(0, xPos));
        res.height = std::stoi(resString.substr(xPos + 1));
    }
    return res;
}

std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &tm);
    return std::string(buffer);
}

int getBatteryPercentage() {
    // Simple placeholder - in real implementation would read from system
    return 71;
}

bool isWiFiConnected() {
    // Simple placeholder - in real implementation would check network status
    return true;
}

// WallpaperRenderer Implementation
WallpaperRenderer::WallpaperRenderer() 
    : textureID_(0), loaded_(false), imageWidth_(0), imageHeight_(0) {
}

WallpaperRenderer::~WallpaperRenderer() {
    if (loaded_ && textureID_) {
        glDeleteTextures(1, &textureID_);
    }
}

bool WallpaperRenderer::loadWallpaper(const std::string& imagePath) {
#ifdef __APPLE__
    // In fallback mode, don't actually load textures
    fprintf(stderr, "Fallback mode: skipping wallpaper texture loading\n");
    imageWidth_ = 1920;
    imageHeight_ = 1080;
    loaded_ = true;
    textureID_ = 1; // Dummy texture ID
    return true;
#else
    // Full OpenGL texture loading
    int channels;
    unsigned char* data = stbi_load(imagePath.c_str(), &imageWidth_, &imageHeight_, &channels, 4);
    
    if (!data) {
        // Create a gradient wallpaper as fallback
        imageWidth_ = 1920;
        imageHeight_ = 1080;
        data = (unsigned char*)malloc(imageWidth_ * imageHeight_ * 4);
        
        for (int y = 0; y < imageHeight_; y++) {
            for (int x = 0; x < imageWidth_; x++) {
                int idx = (y * imageWidth_ + x) * 4;
                // Create a nice gradient
                float r = 0.4f + 0.2f * sin(x * 0.001f);
                float g = 0.6f + 0.2f * sin(y * 0.001f + 1.0f);
                float b = 0.8f + 0.2f * sin((x + y) * 0.001f + 2.0f);
                
                data[idx] = (unsigned char)(r * 255);
                data[idx + 1] = (unsigned char)(g * 255);
                data[idx + 2] = (unsigned char)(b * 255);
                data[idx + 3] = 255;
            }
        }
    }
    
    glGenTextures(1, &textureID_);
    glBindTexture(GL_TEXTURE_2D, textureID_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth_, imageHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    stbi_image_free(data);
    loaded_ = true;
    return true;
#endif
}

void WallpaperRenderer::render(const Resolution& resolution) {
    if (!loaded_) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID_);
    
    // Calculate aspect ratio and center crop
    float imageAspect = (float)imageWidth_ / imageHeight_;
    float screenAspect = (float)resolution.width / resolution.height;
    
    float texLeft, texRight, texTop, texBottom;
    
    if (imageAspect > screenAspect) {
        // Image is wider - crop sides
        float crop = (imageAspect - screenAspect) / (2.0f * imageAspect);
        texLeft = crop;
        texRight = 1.0f - crop;
        texTop = 0.0f;
        texBottom = 1.0f;
    } else {
        // Image is taller - crop top/bottom
        float crop = (screenAspect - imageAspect) / (2.0f * screenAspect);
        texLeft = 0.0f;
        texRight = 1.0f;
        texTop = crop;
        texBottom = 1.0f - crop;
    }
    
    glBegin(GL_QUADS);
    glTexCoord2f(texLeft, texTop); glVertex2f(0, 0);
    glTexCoord2f(texRight, texTop); glVertex2f(resolution.width, 0);
    glTexCoord2f(texRight, texBottom); glVertex2f(resolution.width, resolution.height);
    glTexCoord2f(texLeft, texBottom); glVertex2f(0, resolution.height);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void WallpaperRenderer::bindTexture() {
    if (loaded_) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID_);
    }
}

void WallpaperRenderer::unbindTexture() {
    glDisable(GL_TEXTURE_2D);
}

// BlurShader Implementation
BlurShader::BlurShader() 
    : program_(0), vertexShader_(0), fragmentShader_(0), 
      framebuffer_(0), quadVBO_(0) {
    
    vertexShaderSource_ = R"(
        #version 120
        attribute vec2 a_position;
        attribute vec2 a_texCoord;
        varying vec2 v_texCoord;
        
        void main() {
            gl_Position = vec4(a_position, 0.0, 1.0);
            v_texCoord = a_texCoord;
        }
    )";
    
    fragmentShaderSource_ = R"(
        #version 120
        uniform sampler2D u_texture;
        uniform vec2 u_texelSize;
        uniform float u_radius;
        varying vec2 v_texCoord;
        
        void main() {
            vec4 color = vec4(0.0);
            float total = 0.0;
            
            for (float x = -4.0; x <= 4.0; x += 1.0) {
                for (float y = -4.0; y <= 4.0; y += 1.0) {
                    float weight = exp(-(x*x + y*y) / (2.0 * u_radius * u_radius));
                    vec2 offset = vec2(x, y) * u_texelSize;
                    color += texture2D(u_texture, v_texCoord + offset) * weight;
                    total += weight;
                }
            }
            
            gl_FragColor = color / total;
        }
    )";
}

BlurShader::~BlurShader() {
    cleanup();
}

bool BlurShader::initialize() {
#ifdef __APPLE__
    // In fallback mode, don't initialize OpenGL shaders
    fprintf(stderr, "Fallback mode: skipping blur shader initialization\n");
    return false; // This is expected in fallback mode
#else
    // Full OpenGL shader initialization
    fprintf(stderr, "Initializing OpenGL blur shader...\n");
    
    // Compile shaders
    vertexShader_ = compileShader(GL_VERTEX_SHADER, vertexShaderSource_);
    fragmentShader_ = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource_);
    
    if (!vertexShader_ || !fragmentShader_) return false;
    
    program_ = linkProgram(vertexShader_, fragmentShader_);
    if (!program_) return false;
    
    // Setup quad geometry without VAOs for compatibility
    float quadVertices[] = {
        // Position, TexCoord
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    
    glGenBuffers(1, &quadVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Setup framebuffer
    glGenFramebuffers(1, &framebuffer_);
    
    fprintf(stderr, "OpenGL blur shader initialized successfully\n");
    return true;
#endif
}

void BlurShader::cleanup() {
    if (program_) glDeleteProgram(program_);
    if (vertexShader_) glDeleteShader(vertexShader_);
    if (fragmentShader_) glDeleteShader(fragmentShader_);
    if (quadVBO_) glDeleteBuffers(1, &quadVBO_);
    if (framebuffer_) glDeleteFramebuffers(1, &framebuffer_);
}

GLuint BlurShader::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        fprintf(stderr, "Shader compilation error: %s\n", log);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint BlurShader::linkProgram(GLuint vertex, GLuint fragment) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        fprintf(stderr, "Program linking error: %s\n", log);
        glDeleteProgram(program);
        return 0;
    }
    
    return program;
}

void BlurShader::applyBlur(GLuint sourceTexture, GLuint targetTexture, 
                           int width, int height, float blurRadius) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0);
    
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program_);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTexture);
    glUniform1i(glGetUniformLocation(program_, "u_texture"), 0);
    glUniform2f(glGetUniformLocation(program_, "u_texelSize"), 1.0f/width, 1.0f/height);
    glUniform1f(glGetUniformLocation(program_, "u_radius"), blurRadius);
    
    // Setup vertex attributes without VAO
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// StatusBar Implementation
StatusBar::StatusBar(const LayoutConstraints& constraints) 
    : constraints_(constraints), batteryPercentage_(getBatteryPercentage()), 
      wifiConnected_(isWiFiConnected()) {
    updateTime();
}

void StatusBar::render(const Resolution& resolution) {
    // Transparent background - no fill needed
    
    renderTime(resolution);
    renderBattery(resolution);
    renderWiFi(resolution);
}

void StatusBar::updateTime() {
    currentTime_ = getCurrentTimeString();
}

void StatusBar::renderTime(const Resolution& resolution) {
    // Simple text rendering using bitmap fonts would go here
    // For now, just a placeholder
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(20, resolution.height - constraints_.statusBarHeight / 2 + 5);
    
    // In a real implementation, would render the actual time text
    // For now, we'll skip text rendering as it requires additional font handling
}

void StatusBar::renderBattery(const Resolution& resolution) {
    int batteryX = resolution.width - 80;
    int batteryY = resolution.height - constraints_.statusBarHeight / 2 - 8;
    
    // Battery outline
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(batteryX, batteryY);
    glVertex2f(batteryX + 30, batteryY);
    glVertex2f(batteryX + 30, batteryY + 16);
    glVertex2f(batteryX, batteryY + 16);
    glEnd();
    
    // Battery tip
    glBegin(GL_LINE_LOOP);
    glVertex2f(batteryX + 30, batteryY + 4);
    glVertex2f(batteryX + 34, batteryY + 4);
    glVertex2f(batteryX + 34, batteryY + 12);
    glVertex2f(batteryX + 30, batteryY + 12);
    glEnd();
    
    // Battery fill
    float fillPercent = batteryPercentage_ / 100.0f;
    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(batteryX + 2, batteryY + 2);
    glVertex2f(batteryX + 2 + 26 * fillPercent, batteryY + 2);
    glVertex2f(batteryX + 2 + 26 * fillPercent, batteryY + 14);
    glVertex2f(batteryX + 2, batteryY + 14);
    glEnd();
    
    // Battery percentage text would go here
}

void StatusBar::renderWiFi(const Resolution& resolution) {
    int wifiX = resolution.width - 120;
    int wifiY = resolution.height - constraints_.statusBarHeight / 2 - 8;
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Simple WiFi signal representation
    for (int i = 0; i < 3; i++) {
        int arcHeight = 4 + i * 4;
        glBegin(GL_LINE_STRIP);
        for (int angle = 180; angle <= 360; angle += 10) {
            float rad = angle * M_PI / 180.0f;
            glVertex2f(wifiX + 15 + cos(rad) * (8 + i * 3), 
                      wifiY + 16 + sin(rad) * (8 + i * 3));
        }
        glEnd();
    }
}

// DockIcon Implementation
DockIcon::DockIcon(const std::string& name, const std::string& color) 
    : name_(name), color_(color) {
    initial_ = name.empty() ? '?' : toupper(name[0]);
}

void DockIcon::render(int x, int y, int size, int cornerRadius) {
    // Parse color (simple hex parsing)
    float r = 0.3f, g = 0.6f, b = 1.0f; // Default blue
    
    if (color_.size() > 1 && color_[0] == '#') {
        unsigned long color = std::stoul(color_.substr(1), nullptr, 16);
        r = ((color >> 16) & 0xFF) / 255.0f;
        g = ((color >> 8) & 0xFF) / 255.0f;
        b = (color & 0xFF) / 255.0f;
    }
    
    // Draw rounded rectangle background
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    // Main rectangle
    glVertex2f(x + cornerRadius, y);
    glVertex2f(x + size - cornerRadius, y);
    glVertex2f(x + size - cornerRadius, y + size);
    glVertex2f(x + cornerRadius, y + size);
    
    // Left corner fill
    glVertex2f(x, y + cornerRadius);
    glVertex2f(x + cornerRadius, y + cornerRadius);
    glVertex2f(x + cornerRadius, y + size - cornerRadius);
    glVertex2f(x, y + size - cornerRadius);
    
    // Right corner fill
    glVertex2f(x + size - cornerRadius, y + cornerRadius);
    glVertex2f(x + size, y + cornerRadius);
    glVertex2f(x + size, y + size - cornerRadius);
    glVertex2f(x + size - cornerRadius, y + size - cornerRadius);
    glEnd();
    
    // Draw border
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + cornerRadius, y);
    glVertex2f(x + size - cornerRadius, y);
    glVertex2f(x + size - cornerRadius, y + size);
    glVertex2f(x + cornerRadius, y + size);
    glEnd();
    
    // Draw initial (placeholder for proper text rendering)
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x + size/2 - 4, y + size/2 + 4);
    // In real implementation, would render the actual character
}

// Dock Implementation
Dock::Dock(const LayoutConstraints& constraints, std::shared_ptr<BlurShader> blurShader)
    : constraints_(constraints), blurShader_(blurShader) {
}

void Dock::addIcon(const std::shared_ptr<DockIcon>& icon) {
    icons_.push_back(icon);
}

void Dock::render(const Resolution& resolution, GLuint wallpaperTexture) {
    renderBackground(resolution, wallpaperTexture);
    renderIcons(resolution);
}

int Dock::getDockY(const Resolution& resolution) const {
    return resolution.height - constraints_.dockHeight - 20;
}

int Dock::getDockWidth(const Resolution& resolution) const {
    int iconCount = std::min((int)icons_.size(), 4); // Max 4 icons
    return iconCount * constraints_.iconSize + (iconCount - 1) * constraints_.dockIconSpacing 
           + constraints_.dockPaddingX * 2;
}

int Dock::getDockX(const Resolution& resolution) const {
    return (resolution.width - getDockWidth(resolution)) / 2;
}

void Dock::renderBackground(const Resolution& resolution, GLuint wallpaperTexture) {
    int dockX = getDockX(resolution);
    int dockY = getDockY(resolution);
    int dockWidth = getDockWidth(resolution);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw semi-transparent background
    glColor4f(0.0f, 0.0f, 0.0f, constraints_.dockOpacity);
    
    // Draw rounded rectangle for dock
    int radius = constraints_.dockCornerRadius;
    
    glBegin(GL_QUADS);
    // Main rectangle
    glVertex2f(dockX + radius, dockY);
    glVertex2f(dockX + dockWidth - radius, dockY);
    glVertex2f(dockX + dockWidth - radius, dockY + constraints_.dockHeight);
    glVertex2f(dockX + radius, dockY + constraints_.dockHeight);
    
    // Left corner
    glVertex2f(dockX, dockY + radius);
    glVertex2f(dockX + radius, dockY + radius);
    glVertex2f(dockX + radius, dockY + constraints_.dockHeight - radius);
    glVertex2f(dockX, dockY + constraints_.dockHeight - radius);
    
    // Right corner
    glVertex2f(dockX + dockWidth - radius, dockY + radius);
    glVertex2f(dockX + dockWidth, dockY + radius);
    glVertex2f(dockX + dockWidth, dockY + constraints_.dockHeight - radius);
    glVertex2f(dockX + dockWidth - radius, dockY + constraints_.dockHeight - radius);
    glEnd();
    
    // Draw top highlight
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(dockX + radius, dockY);
    glVertex2f(dockX + dockWidth - radius, dockY);
    glEnd();
    
    // Draw shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(dockX + radius, dockY + constraints_.dockHeight);
    glVertex2f(dockX + dockWidth - radius, dockY + constraints_.dockHeight);
    glEnd();
    
    glDisable(GL_BLEND);
}

void Dock::renderIcons(const Resolution& resolution) {
    int dockX = getDockX(resolution);
    int dockY = getDockY(resolution);
    int startX = dockX + constraints_.dockPaddingX;
    
    for (size_t i = 0; i < icons_.size() && i < 4; i++) {
        int iconX = startX + i * (constraints_.iconSize + constraints_.dockIconSpacing);
        int iconY = dockY + (constraints_.dockHeight - constraints_.iconSize) / 2;
        
        icons_[i]->render(iconX, iconY, constraints_.iconSize, constraints_.iconCornerRadius);
    }
}

// FreeBoardUI Implementation
FreeBoardUI::FreeBoardUI() 
    : display_(nullptr), window_(0), glContext_(nullptr) {
}

FreeBoardUI::~FreeBoardUI() {
    cleanup();
}

bool FreeBoardUI::initialize(const Resolution& resolution) {
    resolution_ = resolution;
    
    fprintf(stderr, "Initializing FreeBoard UI...\n");
    
    if (!initializeOpenGL()) {
        fprintf(stderr, "Failed to initialize OpenGL\n");
        return false;
    }
    
    fprintf(stderr, "OpenGL initialized successfully\n");
    
    // Initialize components with error checking
    fprintf(stderr, "Creating UI components...\n");
    
    try {
        wallpaper_ = std::make_shared<WallpaperRenderer>();
        fprintf(stderr, "Wallpaper renderer created\n");
        
        blurShader_ = std::make_shared<BlurShader>();
        fprintf(stderr, "Blur shader created\n");
        
        statusBar_ = std::make_shared<StatusBar>(constraints_);
        fprintf(stderr, "Status bar created\n");
        
        dock_ = std::make_shared<Dock>(constraints_, blurShader_);
        fprintf(stderr, "Dock created\n");
        
        fprintf(stderr, "UI components created\n");
    } catch (const std::exception& e) {
        fprintf(stderr, "Exception creating UI components: %s\n", e.what());
        return false;
    } catch (...) {
        fprintf(stderr, "Unknown exception creating UI components\n");
        return false;
    }
    
    // Initialize shader with error checking
    fprintf(stderr, "Initializing blur shader...\n");
    if (!blurShader_->initialize()) {
        fprintf(stderr, "Failed to initialize blur shader - continuing without blur\n");
        // Continue without blur shader - it's not critical for basic functionality
    } else {
        fprintf(stderr, "Blur shader initialized\n");
    }
    
    // Load wallpaper with fallback
    fprintf(stderr, "Loading wallpaper...\n");
    if (!wallpaper_->loadWallpaper("/usr/share/pixmaps/wallpaper.png")) {
        fprintf(stderr, "Using gradient wallpaper fallback\n");
        if (!wallpaper_->loadWallpaper("")) {
            fprintf(stderr, "Failed to create fallback wallpaper\n");
            return false;
        }
    }
    fprintf(stderr, "Wallpaper loaded successfully\n");
    
    // Create default icons
    fprintf(stderr, "Creating default icons...\n");
    createDefaultIcons();
    fprintf(stderr, "Default icons created\n");
    
    fprintf(stderr, "FreeBoard UI initialization complete\n");
    return true;
}

void FreeBoardUI::cleanup() {
#ifdef __APPLE__
    if (glContext_) {
        // macOS cleanup would go here
        glContext_ = nullptr;
    }
#else
    if (glContext_) {
        glXDestroyContext(display_, glContext_);
        glContext_ = nullptr;
    }
#endif
    
    if (window_) {
        XDestroyWindow(display_, window_);
        window_ = 0;
    }
    
    if (display_) {
        XCloseDisplay(display_);
        display_ = nullptr;
    }
}

void FreeBoardUI::render() {
    if (!display_ || !window_) {
        fprintf(stderr, "Invalid display or window in render\n");
        return;
    }
    
#ifdef __APPLE__
    // On macOS with fallback mode, use basic X11 rendering
    if (!renderFallback()) {
        fprintf(stderr, "Fallback rendering failed\n");
        return;
    }
#else
    // Full OpenGL rendering on Linux/Unix
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, resolution_.width, 0, resolution_.height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Render wallpaper
    if (wallpaper_) {
        wallpaper_->render(resolution_);
    }
    
    // Render status bar
    if (statusBar_) {
        statusBar_->render(resolution_);
    }
    
    // Render dock with blur effect
    if (dock_) {
        dock_->render(resolution_, wallpaper_ ? wallpaper_->getTextureID() : 0);
    }
    
    glXSwapBuffers(display_, window_);
#endif
}

bool FreeBoardUI::renderFallback() {
    // Simple X11 fallback rendering for macOS
    if (!display_ || !window_) {
        return false;
    }
    
    // Create a simple graphics context
    GC gc = XCreateGC(display_, window_, 0, nullptr);
    if (!gc) {
        return false;
    }
    
    // Clear window with dark background
    XSetWindowBackground(display_, window_, BlackPixel(display_, DefaultScreen(display_)));
    XClearWindow(display_, window_);
    
    // Draw simple status bar
    int statusHeight = 40;
    XSetForeground(display_, gc, WhitePixel(display_, DefaultScreen(display_)));
    XDrawString(display_, window_, gc, 20, statusHeight - 10, "FreeBoard - Fallback Mode", 24);
    
    // Draw simple dock
    int dockHeight = 120;
    int dockY = resolution_.height - dockHeight - 20;
    int dockWidth = 400;
    int dockX = (resolution_.width - dockWidth) / 2;
    
    // Dock background
    XSetForeground(display_, gc, 0x404040); // Gray
    XFillRectangle(display_, window_, gc, dockX, dockY, dockWidth, dockHeight);
    
    // Dock border
    XSetForeground(display_, gc, WhitePixel(display_, DefaultScreen(display_)));
    XDrawRectangle(display_, window_, gc, dockX, dockY, dockWidth, dockHeight);
    
    // Simple placeholder icons
    int iconSize = 60;
    int iconSpacing = 12;
    int startX = dockX + 40;
    
    const char* iconNames[] = {"A", "B", "C", "D"};
    unsigned long iconColors[] = {0x007AFF, 0xFF9500, 0x8E8E93, 0xFFFFFF};
    
    for (int i = 0; i < 4; i++) {
        int iconX = startX + i * (iconSize + iconSpacing);
        int iconY = dockY + (dockHeight - iconSize) / 2;
        
        // Icon background
        XSetForeground(display_, gc, iconColors[i]);
        XFillRectangle(display_, window_, gc, iconX, iconY, iconSize, iconSize);
        
        // Icon text
        XSetForeground(display_, gc, BlackPixel(display_, DefaultScreen(display_)));
        XDrawString(display_, window_, gc, iconX + iconSize/2 - 5, iconY + iconSize/2 + 5, iconNames[i], 1);
    }
    
    // Free graphics context
    XFreeGC(display_, gc);
    
    // Flush to ensure drawing is visible
    XFlush(display_);
    
    return true;
}

void FreeBoardUI::handleResize(int width, int height) {
    resolution_.width = width;
    resolution_.height = height;
    glViewport(0, 0, width, height);
}

bool FreeBoardUI::loadWallpaper(const std::string& imagePath) {
    return wallpaper_ && wallpaper_->loadWallpaper(imagePath);
}

bool FreeBoardUI::initializeOpenGL() {
    display_ = XOpenDisplay(nullptr);
    if (!display_) {
        fprintf(stderr, "Cannot open X display\n");
        return false;
    }
    
    int screen = DefaultScreen(display_);
    Window root = RootWindow(display_, screen);
    
#ifdef __APPLE__
    // For macOS with X11, create a simple window and test basic functionality first
    fprintf(stderr, "Initializing on macOS with X11...\n");
    
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask;
    swa.colormap = DefaultColormap(display_, screen);
    
    window_ = XCreateWindow(display_, root, 0, 0, resolution_.width, resolution_.height,
                           0, CopyFromParent, InputOutput, CopyFromParent,
                           CWEventMask | CWColormap, &swa);
    
    if (!window_) {
        fprintf(stderr, "Failed to create X11 window\n");
        XCloseDisplay(display_);
        return false;
    }
    
    XMapWindow(display_, window_);
    XStoreName(display_, window_, "FreeBoard");
    XFlush(display_);
    
    // Test basic X11 functionality first
    XEvent event;
    if (!XCheckWindowEvent(display_, window_, ExposureMask, &event)) {
        fprintf(stderr, "Warning: No exposure event received\n");
    }
    
    // Initialize basic OpenGL context (simplified for macOS X11)
    glContext_ = (void*)1; // Placeholder - real OpenGL would need proper GLX setup
    
    fprintf(stderr, "Warning: Using fallback mode on macOS - full OpenGL not available\n");
    fprintf(stderr, "Basic X11 window created successfully\n");
#else
    // Linux/Unix with GLX
    fprintf(stderr, "Initializing OpenGL with GLX...\n");
    
    static int visual_attribs[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_RED_SIZE,   8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE,  8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        None
    };
    
    XVisualInfo* vi = glXChooseVisual(display_, screen, visual_attribs);
    if (!vi) {
        fprintf(stderr, "Cannot find suitable visual\n");
        XCloseDisplay(display_);
        return false;
    }
    
    // Create window
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(display_, root, vi->visual, AllocNone);
    swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask;
    
    window_ = XCreateWindow(display_, root, 0, 0, resolution_.width, resolution_.height,
                           0, vi->depth, InputOutput, vi->visual,
                           CWColormap | CWEventMask, &swa);
    
    if (!window_) {
        fprintf(stderr, "Failed to create GLX window\n");
        XCloseDisplay(display_);
        return false;
    }
    
    XMapWindow(display_, window_);
    XStoreName(display_, window_, "FreeBoard");
    
    // Create GL context
    glContext_ = glXCreateContext(display_, vi, nullptr, True);
    if (!glContext_) {
        fprintf(stderr, "Cannot create GL context\n");
        XDestroyWindow(display_, window_);
        XCloseDisplay(display_);
        return false;
    }
    
    if (!glXMakeCurrent(display_, window_, glContext_)) {
        fprintf(stderr, "Cannot make GL context current\n");
        glXDestroyContext(display_, glContext_);
        XDestroyWindow(display_, window_);
        XCloseDisplay(display_);
        return false;
    }
    
    fprintf(stderr, "OpenGL context created successfully\n");
#endif
    
    // Test basic OpenGL functionality only in non-fallback mode
#ifndef __APPLE__
    // Test basic OpenGL functionality
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Enable basic OpenGL features
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    
    fprintf(stderr, "Basic OpenGL features enabled\n");
#else
    fprintf(stderr, "Skipping OpenGL tests in fallback mode\n");
#endif
    
    return true;
}

void FreeBoardUI::createDefaultIcons() {
    // Create placeholder icons similar to reference image
    dock_->addIcon(std::make_shared<DockIcon>("App Store", "#007AFF"));
    dock_->addIcon(std::make_shared<DockIcon>("Books", "#FF9500"));
    dock_->addIcon(std::make_shared<DockIcon>("Calculator", "#8E8E93"));
    dock_->addIcon(std::make_shared<DockIcon>("Calendar", "#FFFFFF"));
}

} // namespace FreeBoardUI
