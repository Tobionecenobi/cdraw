/**
 * p5c_single.h - Cross-platform drawing library inspired by p5.js (Header-only version)
 *
 * A simple C library for creative coding that works on both
 * Windows (using Win32 API) and Linux (using Xlib)
 *
 * This is a single-header version of the p5c library that includes all the implementation.
 * To use it, include this file in your project and define P5C_IMPLEMENTATION in exactly one
 * source file before including this header.
 *
 * Example:
 *
 * // In your main.c file:
 * #define P5C_IMPLEMENTATION
 * #include "p5c_single.h"
 *
 * void setup() {
 *     size(800, 600);
 * }
 *
 * void draw() {
 *     background(0, 0, 0);
 *     // Your drawing code here
 * }
 *
 * int main() {
 *     return run();
 * }
 */

#ifndef P5C_SINGLE_H
#define P5C_SINGLE_H

#include <stdint.h>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define P5C_WINDOWS
#elif defined(__linux__)
    #define P5C_LINUX
#else
    #error "Unsupported platform"
#endif

// Basic types
typedef struct {
    uint8_t r, g, b;
} Color;

// Global state variables
extern int width;
extern int height;
extern int frameCount;
extern int mouseX;
extern int mouseY;
extern int mousePressed;
extern char key;
extern int keyPressed;

// Core functions to be implemented by the user
void setup(void);
void draw(void);

// Library initialization and control
void size(int w, int h);
void background(uint8_t r, uint8_t g, uint8_t b);
void frameRate(int fps);
int run(void);

// Drawing functions
void point(int x, int y);
void line(int x1, int y1, int x2, int y2);
void rect(int x, int y, int w, int h);
void ellipse(int x, int y, int w, int h);
void triangle(int x1, int y1, int x2, int y2, int x3, int y3);

// Color functions
void fill(uint8_t r, uint8_t g, uint8_t b);
void stroke(uint8_t r, uint8_t g, uint8_t b);
void noFill(void);
void noStroke(void);

// Math utilities
float map(float value, float start1, float stop1, float start2, float stop2);
float randomf(float min, float max);  // Renamed to avoid conflict with stdlib
float constrain(float value, float min, float max);
float dist(float x1, float y1, float x2, float y2);

// Input functions
int keyIsDown(char k);

#ifdef P5C_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Platform-specific includes
#ifdef P5C_WINDOWS
    #include <windows.h>
#endif

#ifdef P5C_LINUX
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <unistd.h>
#endif

// Global state variables
int width = 640;
int height = 480;
int frameCount = 0;
int mouseX = 0;
int mouseY = 0;
int mousePressed = 0;
char key = 0;
int keyPressed = 0;

// Key state array (ASCII values 0-255)
static int keyStates[256] = {0};

// Internal state
static uint32_t* framebuffer = NULL;
static Color fillColor = {255, 255, 255};
static Color strokeColor = {0, 0, 0};
static int useFill = 1;
static int useStroke = 1;
static int targetFrameRate = 60;

// Function pointers for user-defined functions
static void (*_setup)(void) = NULL;
static void (*_draw)(void) = NULL;

// Platform-specific variables
#ifdef P5C_WINDOWS
    static HWND hwnd;
    static HDC hdc;
    static BITMAPINFO bmi;
#endif

#ifdef P5C_LINUX
    static Display* display;
    static Window window;
    static GC gc;
    static XImage* ximage;
    static Atom wm_delete_window;
#endif

// Forward declarations of internal functions
static void _init_framebuffer(void);
static void _render_framebuffer(void);
static void _clear_framebuffer(uint8_t r, uint8_t g, uint8_t b);
static void _set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
static void _sort_points_by_y(int* x1, int* y1, int* x2, int* y2, int* x3, int* y3);

// Initialize the library
void size(int w, int h) {
    width = w;
    height = h;
}

// Set the background color
void background(uint8_t r, uint8_t g, uint8_t b) {
    _clear_framebuffer(r, g, b);
}

// Set the target frame rate
void frameRate(int fps) {
    targetFrameRate = fps > 0 ? fps : 60;
}

// Set the fill color
void fill(uint8_t r, uint8_t g, uint8_t b) {
    fillColor.r = r;
    fillColor.g = g;
    fillColor.b = b;
    useFill = 1;
}

// Disable filling
void noFill(void) {
    useFill = 0;
}

// Set the stroke color
void stroke(uint8_t r, uint8_t g, uint8_t b) {
    strokeColor.r = r;
    strokeColor.g = g;
    strokeColor.b = b;
    useStroke = 1;
}

// Disable stroke
void noStroke(void) {
    useStroke = 0;
}

// Draw a single point
void point(int x, int y) {
    if (useStroke) {
        _set_pixel(x, y, strokeColor.r, strokeColor.g, strokeColor.b);
    }
}

// Draw a line using Bresenham's algorithm
void line(int x1, int y1, int x2, int y2) {
    if (!useStroke) return;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (1) {
        _set_pixel(x1, y1, strokeColor.r, strokeColor.g, strokeColor.b);

        if (x1 == x2 && y1 == y2) break;

        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draw a rectangle
void rect(int x, int y, int w, int h) {
    // Fill the rectangle if fill is enabled
    if (useFill) {
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                _set_pixel(i, j, fillColor.r, fillColor.g, fillColor.b);
            }
        }
    }

    // Draw the outline if stroke is enabled
    if (useStroke) {
        line(x, y, x + w - 1, y);         // Top
        line(x + w - 1, y, x + w - 1, y + h - 1); // Right
        line(x + w - 1, y + h - 1, x, y + h - 1); // Bottom
        line(x, y + h - 1, x, y);         // Left
    }
}

// Draw an ellipse using the midpoint ellipse algorithm
void ellipse(int x, int y, int w, int h) {
    int a = w / 2;
    int b = h / 2;
    int cx = x + a;
    int cy = y + b;

    // Fill the ellipse if fill is enabled
    if (useFill) {
        // Simple scanline fill
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                float dx = (i - cx) / (float)a;
                float dy = (j - cy) / (float)b;
                if (dx * dx + dy * dy <= 1.0f) {
                    _set_pixel(i, j, fillColor.r, fillColor.g, fillColor.b);
                }
            }
        }
    }

    // Draw the outline if stroke is enabled
    if (useStroke) {
        // Midpoint ellipse algorithm
        int a2 = a * a;
        int b2 = b * b;
        int fa2 = 4 * a2;
        int fb2 = 4 * b2;
        int x1 = 0;
        int y1 = b;
        int sigma = 2 * b2 + a2 * (1 - 2 * b);

        // First half
        while (b2 * x1 <= a2 * y1) {
            _set_pixel(cx + x1, cy + y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x1, cy + y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx + x1, cy - y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x1, cy - y1, strokeColor.r, strokeColor.g, strokeColor.b);

            if (sigma >= 0) {
                sigma += fa2 * (1 - y1);
                y1--;
            }
            sigma += b2 * ((4 * x1) + 6);
            x1++;
        }

        // Second half
        x1 = a;
        y1 = 0;
        sigma = 2 * a2 + b2 * (1 - 2 * a);

        while (a2 * y1 <= b2 * x1) {
            _set_pixel(cx + x1, cy + y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x1, cy + y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx + x1, cy - y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x1, cy - y1, strokeColor.r, strokeColor.g, strokeColor.b);

            if (sigma >= 0) {
                sigma += fb2 * (1 - x1);
                x1--;
            }
            sigma += a2 * ((4 * y1) + 6);
            y1++;
        }
    }
}

// Helper function to sort triangle vertices by y-coordinate
static void _sort_points_by_y(int* x1, int* y1, int* x2, int* y2, int* x3, int* y3) {
    // Sort vertices by y-coordinate (bubble sort)
    if (*y1 > *y2) {
        int tempX = *x1;
        int tempY = *y1;
        *x1 = *x2;
        *y1 = *y2;
        *x2 = tempX;
        *y2 = tempY;
    }
    if (*y2 > *y3) {
        int tempX = *x2;
        int tempY = *y2;
        *x2 = *x3;
        *y2 = *y3;
        *x3 = tempX;
        *y3 = tempY;
    }
    if (*y1 > *y2) {
        int tempX = *x1;
        int tempY = *y1;
        *x1 = *x2;
        *y1 = *y2;
        *x2 = tempX;
        *y2 = tempY;
    }
}

// Draw a triangle
void triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    // Draw the outline if stroke is enabled
    if (useStroke) {
        line(x1, y1, x2, y2);
        line(x2, y2, x3, y3);
        line(x3, y3, x1, y1);
    }

    // Fill the triangle if fill is enabled
    if (useFill) {
        // Sort the vertices by y-coordinate
        _sort_points_by_y(&x1, &y1, &x2, &y2, &x3, &y3);

        // Handle flat bottom triangle
        if (y2 == y3) {
            // Fill flat bottom triangle
            float slope1 = (float)(x2 - x1) / (y2 - y1);
            float slope2 = (float)(x3 - x1) / (y3 - y1);
            float x_start = x1;
            float x_end = x1;

            for (int y = y1; y <= y2; y++) {
                for (int x = (int)x_start; x <= (int)x_end; x++) {
                    _set_pixel(x, y, fillColor.r, fillColor.g, fillColor.b);
                }
                x_start += slope1;
                x_end += slope2;
            }
        }
        // Handle flat top triangle
        else if (y1 == y2) {
            // Fill flat top triangle
            float slope1 = (float)(x3 - x1) / (y3 - y1);
            float slope2 = (float)(x3 - x2) / (y3 - y2);
            float x_start = x3;
            float x_end = x3;

            for (int y = y3; y >= y1; y--) {
                for (int x = (int)x_start; x <= (int)x_end; x++) {
                    _set_pixel(x, y, fillColor.r, fillColor.g, fillColor.b);
                }
                x_start -= slope1;
                x_end -= slope2;
            }
        }
        // Handle general triangle - split into flat bottom and flat top
        else {
            // Calculate the new vertex (x4,y4) that creates the flat side
            int y4 = y2;
            int x4 = x1 + ((y2 - y1) * (x3 - x1)) / (y3 - y1);

            // Fill flat bottom triangle (v1, v2, v4)
            float slope1 = (float)(x2 - x1) / (y2 - y1);
            float slope2 = (float)(x4 - x1) / (y4 - y1);
            float x_start = x1;
            float x_end = x1;

            for (int y = y1; y <= y2; y++) {
                for (int x = (int)x_start; x <= (int)x_end; x++) {
                    _set_pixel(x, y, fillColor.r, fillColor.g, fillColor.b);
                }
                x_start += slope1;
                x_end += slope2;
            }

            // Fill flat top triangle (v2, v4, v3)
            slope1 = (float)(x3 - x2) / (y3 - y2);
            slope2 = (float)(x3 - x4) / (y3 - y4);
            x_start = x3;
            x_end = x3;

            for (int y = y3; y > y2; y--) {
                for (int x = (int)x_start; x <= (int)x_end; x++) {
                    _set_pixel(x, y, fillColor.r, fillColor.g, fillColor.b);
                }
                x_start -= slope1;
                x_end -= slope2;
            }
        }
    }
}

// Initialize the framebuffer
static void _init_framebuffer(void) {
    if (framebuffer) {
        free(framebuffer);
    }
    framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!framebuffer) {
        fprintf(stderr, "Failed to allocate framebuffer\n");
        exit(1);
    }
    _clear_framebuffer(0, 0, 0);
}

// Clear the framebuffer with a specific color
static void _clear_framebuffer(uint8_t r, uint8_t g, uint8_t b) {
    if (!framebuffer) return;

    uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    for (int i = 0; i < width * height; i++) {
        framebuffer[i] = color;
    }
}

// Set a pixel in the framebuffer
static void _set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!framebuffer) return;
    if (x < 0 || x >= width || y < 0 || y >= height) return;

    uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    framebuffer[y * width + x] = color;
}

// Math utilities
float map(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

// Check if a key is currently pressed
int keyIsDown(char k) {
    // Convert to unsigned char to ensure positive index
    unsigned char index = (unsigned char)k;
    return keyStates[index];
}

float randomf(float min, float max) {
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

float constrain(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float dist(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

// Platform-specific window creation and main loop
#ifdef P5C_WINDOWS

// Windows implementation
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_MOUSEMOVE:
            mouseX = LOWORD(lParam);
            mouseY = HIWORD(lParam);
            return 0;
        case WM_LBUTTONDOWN:
            mousePressed = 1;
            return 0;
        case WM_LBUTTONUP:
            mousePressed = 0;
            return 0;
        case WM_KEYDOWN:
            key = (char)wParam;
            keyPressed = 1;
            keyStates[(unsigned char)key] = 1;
            return 0;
        case WM_KEYUP:
            keyPressed = 0;
            keyStates[(unsigned char)key] = 0;
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static void _render_framebuffer(void) {
    StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height,
                 framebuffer, &bmi, DIB_RGB_COLORS, SRCCOPY);
}

int run(void) {
    _setup = setup;
    _draw = draw;

    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "P5CWindowClass";
    RegisterClass(&wc);

    // Create window
    hwnd = CreateWindowEx(
        0, "P5CWindowClass", "P5C Drawing",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hwnd) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    // Adjust window size to account for borders
    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                SWP_NOMOVE | SWP_NOZORDER);

    // Show window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Get device context
    hdc = GetDC(hwnd);

    // Initialize bitmap info
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative for top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Initialize framebuffer
    _init_framebuffer();

    // Call user setup function
    if (_setup) {
        _setup();
    }

    // Main loop
    MSG msg;
    DWORD lastFrameTime = GetTickCount();
    DWORD targetFrameTime = 1000 / targetFrameRate;

    while (1) {
        // Process all pending messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                goto cleanup;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Calculate frame timing
        DWORD currentTime = GetTickCount();
        DWORD elapsedTime = currentTime - lastFrameTime;

        if (elapsedTime >= targetFrameTime) {
            // Call user draw function
            if (_draw) {
                _draw();
            }

            // Render the framebuffer to the screen
            _render_framebuffer();

            // Update frame count and time
            frameCount++;
            lastFrameTime = currentTime;
        } else {
            // Sleep to save CPU
            Sleep(1);
        }
    }

cleanup:
    // Clean up
    if (framebuffer) {
        free(framebuffer);
        framebuffer = NULL;
    }
    ReleaseDC(hwnd, hdc);
    return 0;
}

#endif /* P5C_WINDOWS */

#ifdef P5C_LINUX

// X11 implementation
static void _render_framebuffer(void) {
    if (!ximage || !framebuffer) return;

    // Update the XImage data
    ximage->data = (char*)framebuffer;

    // Put the image on the window
    XPutImage(display, window, gc, ximage, 0, 0, 0, 0, width, height);
    XFlush(display);
}

int run(void) {
    _setup = setup;
    _draw = draw;

    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Open display
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open X display\n");
        return 1;
    }

    // Get default screen
    int screen = DefaultScreen(display);

    // Create window
    window = XCreateSimpleWindow(
        display, RootWindow(display, screen),
        0, 0, width, height, 1,
        BlackPixel(display, screen), BlackPixel(display, screen)
    );

    // Set window title
    XStoreName(display, window, "P5C Drawing");

    // Set window close event
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    // Select input events
    XSelectInput(display, window,
                ExposureMask | KeyPressMask | KeyReleaseMask |
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    // Create GC
    gc = XCreateGC(display, window, 0, NULL);

    // Map window
    XMapWindow(display, window);

    // Create XImage
    Visual* visual = DefaultVisual(display, screen);
    int depth = DefaultDepth(display, screen);

    // Initialize framebuffer
    _init_framebuffer();

    // Create XImage
    ximage = XCreateImage(display, visual, depth, ZPixmap, 0,
                         (char*)framebuffer, width, height, 32, width * 4);

    if (!ximage) {
        fprintf(stderr, "Failed to create XImage\n");
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return 1;
    }

    // Call user setup function
    if (_setup) {
        _setup();
    }

    // Main loop
    XEvent event;
    struct timespec lastFrameTime, currentTime, sleepTime;
    clock_gettime(CLOCK_MONOTONIC, &lastFrameTime);
    long targetFrameTime = 1000000000 / targetFrameRate; // in nanoseconds

    while (1) {
        // Process all pending events
        while (XPending(display)) {
            XNextEvent(display, &event);

            switch (event.type) {
                case Expose:
                    _render_framebuffer();
                    break;
                case ClientMessage:
                    if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                        goto cleanup;
                    }
                    break;
                case MotionNotify:
                    mouseX = event.xmotion.x;
                    mouseY = event.xmotion.y;
                    break;
                case ButtonPress:
                    if (event.xbutton.button == Button1) {
                        mousePressed = 1;
                    }
                    break;
                case ButtonRelease:
                    if (event.xbutton.button == Button1) {
                        mousePressed = 0;
                    }
                    break;
                case KeyPress:
                    keyPressed = 1;
                    key = XLookupKeysym(&event.xkey, 0) & 0xFF;
                    keyStates[(unsigned char)key] = 1;
                    break;
                case KeyRelease:
                    keyPressed = 0;
                    keyStates[(unsigned char)key] = 0;
                    break;
            }
        }

        // Calculate frame timing
        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        long elapsedTime = (currentTime.tv_sec - lastFrameTime.tv_sec) * 1000000000 +
                          (currentTime.tv_nsec - lastFrameTime.tv_nsec);

        if (elapsedTime >= targetFrameTime) {
            // Call user draw function
            if (_draw) {
                _draw();
            }

            // Render the framebuffer to the screen
            _render_framebuffer();

            // Update frame count and time
            frameCount++;
            lastFrameTime = currentTime;
        } else {
            // Sleep to save CPU
            sleepTime.tv_sec = 0;
            sleepTime.tv_nsec = 1000000; // 1ms
            nanosleep(&sleepTime, NULL);
        }
    }

cleanup:
    // Clean up
    if (ximage) {
        ximage->data = NULL; // Prevent XDestroyImage from freeing our framebuffer
        XDestroyImage(ximage);
    }
    if (framebuffer) {
        free(framebuffer);
        framebuffer = NULL;
    }
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}

#endif /* P5C_LINUX */

#endif /* P5C_IMPLEMENTATION */

#endif /* P5C_SINGLE_H */
