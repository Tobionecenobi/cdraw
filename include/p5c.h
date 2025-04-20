/**
 * p5c.h - Cross-platform drawing library inspired by p5.js
 *
 * A simple C library for creative coding that works on both
 * Windows (using Win32 API) and Linux (using Xlib)
 */

#ifndef P5C_H
#define P5C_H

#include <stdint.h>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define P5C_WINDOWS
#elif defined(__linux__)
    #define P5C_LINUX
#else
    #error "Unsupported platform"
#endif

// Special key codes
#define ARROW_UP    0x26
#define ARROW_DOWN  0x28
#define ARROW_LEFT  0x25
#define ARROW_RIGHT 0x27

// Angle modes
#define RADIANS     0
#define DEGREES     1

// Arc modes
#define OPEN        0
#define CHORD       1
#define PIE         2

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
void square(int x, int y, int size);
void ellipse(int x, int y, int w, int h);
void circle(int x, int y, int r);
void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void arc(int x, int y, int w, int h, float start, float stop);
void arcMode(int x, int y, int w, int h, float start, float stop, int mode);
void arcDetail(int x, int y, int w, int h, float start, float stop, int mode, int detail);

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
void angleMode(int mode);

// Input functions
int keyIsDown(char k);

#endif /* P5C_H */
