/**
 * p5c.c - Implementation of the p5c drawing library
 */

#include "../include/p5c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Define CLOCK_MONOTONIC if not available
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
#include <math.h>
#include <string.h> // for memcpy

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

// Key state arrays
// Regular keys (ASCII values 0-255)
static int keyStates[256] = {0};
// Special keys (arrow keys, etc.)
static int specialKeyStates[256] = {0};

// Internal state
static uint32_t* framebuffer = NULL;
static Color fillColor = {255, 255, 255};
static Color strokeColor = {0, 0, 0};
static int useFill = 1;
static int useStroke = 1;
static int targetFrameRate = 60;
static int currentAngleMode = RADIANS; // Default to radians

// Add a global variable for stroke weight
static int strokeWeightValue = 1; // Default stroke weight is 1

#define MAX_MATRIX_STACK 32

typedef struct {
    float m[3][3]; // 3x3 matrix for 2D transformations
} Matrix;

static Matrix currentMatrix;
static Matrix matrixStack[MAX_MATRIX_STACK];
static int matrixStackSize = 0;

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
static void _init_matrix(Matrix* m);
static void _transform_point(float* x, float* y);

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

// Function to set stroke weight
void strokeWeight(int weight) {
    if (weight > 0) {
        strokeWeightValue = weight;
    }
}

// Modify the point function to account for stroke weight
void point(int x, int y) {
    if (useStroke) {
        int halfWeight = strokeWeightValue / 2;
        for (int dx = -halfWeight; dx <= halfWeight; dx++) {
            for (int dy = -halfWeight; dy <= halfWeight; dy++) {
                _set_pixel(x + dx, y + dy, strokeColor.r, strokeColor.g, strokeColor.b);
            }
        }
    }
}

// Modify the line function to account for stroke weight
void line(int x1, int y1, int x2, int y2) {
    if (!useStroke) return;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int e2;

    int halfWeight = strokeWeightValue / 2;

    while (1) {
        // Draw a thicker line by adding perpendicular offsets
        for (int offset = -halfWeight; offset <= halfWeight; offset++) {
            if (dx > dy) {
                _set_pixel(x1, y1 + offset, strokeColor.r, strokeColor.g, strokeColor.b);
            } else {
                _set_pixel(x1 + offset, y1, strokeColor.r, strokeColor.g, strokeColor.b);
            }
        }

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

// Draw a square
void square(int x, int y, int size) {
    rect(x, y, size, size);
}

// Draw an ellipse using the midpoint ellipse algorithm
void ellipse(int x, int y, int w, int h) {
    // Calculate ellipse parameters
    int a = w / 2;
    int b = h / 2;
    int cx = x + a;
    int cy = y + b;

    // Handle degenerate cases
    if (w <= 0 || h <= 0) return;

    // Special case for circle with small radius
    if (w == h && w <= 2) {
        if (useFill) {
            _set_pixel(cx, cy, fillColor.r, fillColor.g, fillColor.b);
        } else if (useStroke) {
            _set_pixel(cx, cy, strokeColor.r, strokeColor.g, strokeColor.b);
        }
        return;
    }

    // For filled ellipses, we'll use a scanline algorithm
    if (useFill) {
        // For each scanline within the ellipse bounds
        for (int sy = -b; sy <= b; sy++) {
            // Calculate the width at this y coordinate
            // Using the ellipse formula: (x/a)^2 + (y/b)^2 = 1
            // Solving for x: x = a * sqrt(1 - (y/b)^2)
            float temp = 1.0f - (float)(sy * sy) / (b * b);
            if (temp < 0) continue;

            int width = (int)(a * sqrt(temp));

            // Draw the horizontal line
            for (int sx = -width; sx <= width; sx++) {
                _set_pixel(cx + sx, cy + sy, fillColor.r, fillColor.g, fillColor.b);
            }
        }
    }

    // For the outline, use the midpoint ellipse algorithm
    if (useStroke) {
        // First region: where the slope is < -1
        int x1 = 0;
        int y1 = b;
        int a_sqr = a * a;
        int b_sqr = b * b;
        int dx = 0;
        int dy = 2 * a_sqr * y1;

        // Decision parameter for region 1
        int d1 = b_sqr - a_sqr * b + a_sqr / 4;

        while (dx < dy) {
            // Plot four symmetric points
            _set_pixel(cx + x1, cy + y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x1, cy + y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx + x1, cy - y1, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x1, cy - y1, strokeColor.r, strokeColor.g, strokeColor.b);

            // Update
            x1++;
            dx += 2 * b_sqr;

            if (d1 < 0) {
                d1 += dx + b_sqr;
            } else {
                y1--;
                dy -= 2 * a_sqr;
                d1 += dx - dy + b_sqr;
            }
        }

        // Second region: where the slope is >= -1
        int x2 = a;
        int y2 = 0;
        dx = 2 * b_sqr * x2;
        dy = 0;

        // Decision parameter for region 2
        int d2 = a_sqr - b_sqr * a + b_sqr / 4;

        while (dx > dy) {
            // Plot four symmetric points
            _set_pixel(cx + x2, cy + y2, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x2, cy + y2, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx + x2, cy - y2, strokeColor.r, strokeColor.g, strokeColor.b);
            _set_pixel(cx - x2, cy - y2, strokeColor.r, strokeColor.g, strokeColor.b);

            // Update
            y2++;
            dy += 2 * a_sqr;

            if (d2 < 0) {
                d2 += dy + a_sqr;
            } else {
                x2--;
                dx -= 2 * b_sqr;
                d2 += dy - dx + a_sqr;
            }
        }
    }
}

//Draw a circle using the ellipse algorithm
void circle(int x, int y, int r) {
    ellipse(x, y, r, r);
}

// Helper function to convert angles based on current angle mode
static float _normalize_angle(float angle) {
    if (currentAngleMode == DEGREES) {
        return angle * M_PI / 180.0f;
    }
    return angle;
}

void arcDetail(int x, int y, int w, int h, float start, float stop, int mode, int detail) {
    if (w <= 0 || h <= 0) return;

    int a = w / 2;
    int b = h / 2;
    int cx = x + a;
    int cy = y + b;

    float startAngle = _normalize_angle(start);
    float stopAngle = _normalize_angle(stop);
    if (stopAngle < startAngle) stopAngle += 2 * M_PI;

    if (detail <= 0) detail = 25;
    if (detail > 360) detail = 360;

    float angleStep = (stopAngle - startAngle) / detail;
    int px[362];
    int py[362];
    int count = 0;

    for (int i = 0; i <= detail; i++) {
        float angle = startAngle + i * angleStep;
        px[count] = cx + (int)(a * cosf(angle));
        py[count] = cy + (int)(b * sinf(angle));
        count++;
    }

    // --- FILL ---
    if (useFill) {
        if (mode == PIE) {
            for (int i = 0; i < count - 1; i++) {
                triangle(cx, cy, px[i], py[i], px[i + 1], py[i + 1]);
            }
        }
        else if (mode == CHORD) {
            // Build a filled polygon strip between arc points and closing chord
            for (int i = 0; i < count - 2; i++) {
                triangle(px[i], py[i], px[i + 1], py[i + 1], px[i + 2], py[i + 2]);
            }
        }
        else { // OPEN
            // Fill the arc segment by vertical scanlines inside bounding ellipse
            for (int sy = -b; sy <= b; sy++) {
                float ry = (float)(sy) / b;
                float dx = a * sqrtf(fmaxf(0.0f, 1.0f - ry * ry));
                int xStart = (int)(-dx);
                int xEnd = (int)(dx);

                for (int sx = xStart; sx <= xEnd; sx++) {
                    float angle = atan2f((float)sy, (float)sx);
                    if (angle < 0) angle += 2 * M_PI;

                    if (angle >= startAngle && angle <= stopAngle) {
                        _set_pixel(cx + sx, cy + sy, fillColor.r, fillColor.g, fillColor.b);
                    }
                }
            }
        }
    }

    // --- STROKE ---
    if (useStroke) {
        for (int i = 0; i < count - 1; i++) {
            line(px[i], py[i], px[i + 1], py[i + 1]);
        }

        if (mode == CHORD) {
            line(px[0], py[0], px[count - 1], py[count - 1]);
        } else if (mode == PIE) {
            line(cx, cy, px[0], py[0]);
            line(cx, cy, px[count - 1], py[count - 1]);
        }
    }
}


// Shorthand wrappers
void arc(int x, int y, int w, int h, float start, float stop) {
    arcDetail(x, y, w, h, start, stop, OPEN, 25);
}

void arcMode(int x, int y, int w, int h, float start, float stop, int mode) {
    arcDetail(x, y, w, h, start, stop, mode, 25);
}

// Helper function to sort three points by y-coordinate
static void _sort_points_by_y(int* x1, int* y1, int* x2, int* y2, int* x3, int* y3) {
    // Sort the points by y-coordinate (bubble sort)
    if (*y1 > *y2) {
        int temp_x = *x1, temp_y = *y1;
        *x1 = *x2; *y1 = *y2;
        *x2 = temp_x; *y2 = temp_y;
    }
    if (*y2 > *y3) {
        int temp_x = *x2, temp_y = *y2;
        *x2 = *x3; *y2 = *y3;
        *x3 = temp_x; *y3 = temp_y;
    }
    if (*y1 > *y2) {
        int temp_x = *x1, temp_y = *y1;
        *x1 = *x2; *y1 = *y2;
        *x2 = temp_x; *y2 = temp_y;
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
    printf("Clearing framebuffer with color (%d, %d, %d)\n", r, g, b);
    if (!framebuffer) return;

    uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    printf("Framebuffer size: %d\n", width * height);
    for (int i = 0; i < width * height; i++) {
        framebuffer[i] = color;
    }
    printf("Framebuffer cleared\n");
}

// Set a pixel in the framebuffer
static void _set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!framebuffer) return;

    // Transform the point
    float fx = (float)x;
    float fy = (float)y;
    _transform_point(&fx, &fy);

    // Convert back to integers
    x = (int)(fx + 0.5f);
    y = (int)(fy + 0.5f);

    // Bounds checking
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
    // For special keys (arrow keys, etc.)
    if (k == ARROW_UP || k == ARROW_DOWN || k == ARROW_LEFT || k == ARROW_RIGHT) {
        return specialKeyStates[(unsigned char)k];
    }

    // For regular ASCII keys
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

// Set the angle mode (RADIANS or DEGREES)
void angleMode(int mode) {
    if (mode == RADIANS || mode == DEGREES) {
        currentAngleMode = mode;
    }
}

// Matrix functions
static void _init_matrix(Matrix* m) {
    // Initialize to identity matrix
    memset(m->m, 0, sizeof(m->m));
    m->m[0][0] = 1.0f;
    m->m[1][1] = 1.0f;
    m->m[2][2] = 1.0f;
}

static void _transform_point(float* x, float* y) {
    float tx = *x * currentMatrix.m[0][0] + *y * currentMatrix.m[0][1] + currentMatrix.m[0][2];
    float ty = *x * currentMatrix.m[1][0] + *y * currentMatrix.m[1][1] + currentMatrix.m[1][2];
    *x = tx;
    *y = ty;
}

void resetMatrix(void) {
    _init_matrix(&currentMatrix);
}

void push(void) {
    if (matrixStackSize >= MAX_MATRIX_STACK) {
        fprintf(stderr, "Error: Matrix stack overflow\n");
        return;
    }
    memcpy(&matrixStack[matrixStackSize], &currentMatrix, sizeof(Matrix));
    matrixStackSize++;
}

void pop(void) {
    if (matrixStackSize <= 0) {
        fprintf(stderr, "Error: Matrix stack underflow\n");
        return;
    }
    matrixStackSize--;
    memcpy(&currentMatrix, &matrixStack[matrixStackSize], sizeof(Matrix));
}

void translate(float x, float y) {
    currentMatrix.m[0][2] += x * currentMatrix.m[0][0] + y * currentMatrix.m[0][1];
    currentMatrix.m[1][2] += x * currentMatrix.m[1][0] + y * currentMatrix.m[1][1];
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
            keyPressed = 1;
            // Check for special keys (arrow keys)
            if (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_LEFT || wParam == VK_RIGHT) {
                // Map Windows virtual key codes to our arrow key constants
                if (wParam == VK_UP) specialKeyStates[ARROW_UP] = 1;
                if (wParam == VK_DOWN) specialKeyStates[ARROW_DOWN] = 1;
                if (wParam == VK_LEFT) specialKeyStates[ARROW_LEFT] = 1;
                if (wParam == VK_RIGHT) specialKeyStates[ARROW_RIGHT] = 1;
            } else {
                // Regular ASCII key
                key = (char)wParam;
                keyStates[(unsigned char)key] = 1;
            }
            return 0;
        case WM_KEYUP:
            keyPressed = 0;
            // Check for special keys (arrow keys)
            if (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_LEFT || wParam == VK_RIGHT) {
                // Map Windows virtual key codes to our arrow key constants
                if (wParam == VK_UP) specialKeyStates[ARROW_UP] = 0;
                if (wParam == VK_DOWN) specialKeyStates[ARROW_DOWN] = 0;
                if (wParam == VK_LEFT) specialKeyStates[ARROW_LEFT] = 0;
                if (wParam == VK_RIGHT) specialKeyStates[ARROW_RIGHT] = 0;
            } else {
                // Regular ASCII key - use the key that was released (wParam), not the last pressed key
                keyStates[(unsigned char)wParam] = 0;
            }
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
    
    // Call user setup function
    if (_setup) {
        _setup();
    }

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

    // Initialize matrix
    resetMatrix();


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
    
    // Call user setup function
    if (_setup) {
        _setup();
    }
    
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

    // Initialize matrix
    resetMatrix();

    // Create XImage
    ximage = XCreateImage(display, visual, depth, ZPixmap, 0,
                         (char*)framebuffer, width, height, 32, width * 4);

    if (!ximage) {
        fprintf(stderr, "Failed to create XImage\n");
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return 1;
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
                    {
                        KeySym keysym = XLookupKeysym(&event.xkey, 0);

                        // Check for special keys (arrow keys)
                        if (keysym == XK_Up || keysym == XK_Down || keysym == XK_Left || keysym == XK_Right) {
                            // Map X11 keysyms to our arrow key constants
                            if (keysym == XK_Up) specialKeyStates[ARROW_UP] = 1;
                            if (keysym == XK_Down) specialKeyStates[ARROW_DOWN] = 1;
                            if (keysym == XK_Left) specialKeyStates[ARROW_LEFT] = 1;
                            if (keysym == XK_Right) specialKeyStates[ARROW_RIGHT] = 1;
                        } else {
                            // Regular ASCII key
                            key = keysym & 0xFF;
                            keyStates[(unsigned char)key] = 1;
                        }
                    }
                    break;
                case KeyRelease:
                    keyPressed = 0;
                    {
                        KeySym keysym = XLookupKeysym(&event.xkey, 0);

                        // Check for special keys (arrow keys)
                        if (keysym == XK_Up || keysym == XK_Down || keysym == XK_Left || keysym == XK_Right) {
                            // Map X11 keysyms to our arrow key constants
                            if (keysym == XK_Up) specialKeyStates[ARROW_UP] = 0;
                            if (keysym == XK_Down) specialKeyStates[ARROW_DOWN] = 0;
                            if (keysym == XK_Left) specialKeyStates[ARROW_LEFT] = 0;
                            if (keysym == XK_Right) specialKeyStates[ARROW_RIGHT] = 0;
                        } else {
                            // Regular ASCII key - use the key that was released (keysym), not the last pressed key
                            unsigned char released_key = keysym & 0xFF;
                            keyStates[released_key] = 0;
                        }
                    }
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
