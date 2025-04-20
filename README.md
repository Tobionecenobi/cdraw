# p5c - Cross-Platform Drawing Engine

p5c is a lightweight, cross-platform drawing library inspired by [p5.js](https://p5js.org/), implemented in pure C. It allows you to create visual applications and animations that run on both Windows and Linux without any external dependencies.

## Features

- **Cross-platform**: Works on Windows (using Win32 API) and Linux (using Xlib)
- **No external dependencies**: Uses only the native platform APIs
- **Simple API**: Inspired by p5.js, making it easy to learn and use
- **Core drawing primitives**: Points, lines, rectangles, ellipses, and triangles
- **Color control**: Fill and stroke settings with RGB colors
- **Input handling**: Mouse and keyboard input
- **Math utilities**: Helpful functions like map, random, constrain, and distance
- **Header-only option**: Can be used as a single header file

## Examples

### Bouncing Ball
```c
#include "include/p5c.h"

// Global variables for animation
float x, y;
float xspeed = 2.5;
float yspeed = 2;
int radius = 20;

void setup() {
    // Set the canvas size
    size(640, 480);
    
    // Initialize position
    x = width / 2;
    y = height / 2;
    
    // Set frame rate
    frameRate(60);
}

void draw() {
    // Set the background color (RGB)
    background(51, 51, 51);
    
    // Update position
    x += xspeed;
    y += yspeed;
    
    // Check for bouncing
    if (x > width - radius || x < radius) {
        xspeed *= -1;
    }
    if (y > height - radius || y < radius) {
        yspeed *= -1;
    }
    
    // Draw a circle at the current position
    fill(255, 0, 0);
    stroke(255, 255, 255);
    ellipse(x - radius, y - radius, radius * 2, radius * 2);
}

int main() {
    return run();
}
```

### Random Walker
```c
#include "include/p5c.h"

// Walker position
int walkerX, walkerY;

void setup() {
    // Set the canvas size
    size(640, 480);
    
    // Start in the middle
    walkerX = width / 2;
    walkerY = height / 2;
    
    // Set frame rate
    frameRate(30);
}

void draw() {
    // Set the background color
    background(0, 0, 0);
    
    // Set the stroke color to white
    stroke(255, 255, 255);
    
    // Draw the current position
    point(walkerX, walkerY);
    
    // Randomly move the walker
    int choice = (int)randomf(0, 4);
    
    switch (choice) {
        case 0: // Up
            walkerY--;
            break;
        case 1: // Right
            walkerX++;
            break;
        case 2: // Down
            walkerY++;
            break;
        case 3: // Left
            walkerX--;
            break;
    }
    
    // Constrain to the screen
    walkerX = (int)constrain(walkerX, 0, width - 1);
    walkerY = (int)constrain(walkerY, 0, height - 1);
}

int main() {
    return run();
}
```

## Building

### Linux
Requirements:
- GCC
- X11 development libraries (`libx11-dev`)

```bash
# Install X11 development libraries if needed
sudo apt-get install libx11-dev

# Build the library and examples
make

# Run an example
./build/main
```

### Windows
Requirements:
- MinGW or similar GCC-compatible compiler
- Windows SDK

```bash
# Build the library and examples
mingw32-make

# Run an example
build\main.exe
```

## API Reference

### Core Functions
- `void setup()` - Called once at the beginning of execution
- `void draw()` - Called repeatedly for each frame
- `int run()` - Starts the application

### Canvas Control
- `void size(int w, int h)` - Set the canvas size
- `void background(uint8_t r, uint8_t g, uint8_t b)` - Set the background color
- `void frameRate(int fps)` - Set the target frame rate

### Drawing Primitives
- `void point(int x, int y)` - Draw a point
- `void line(int x1, int y1, int x2, int y2)` - Draw a line
- `void rect(int x, int y, int w, int h)` - Draw a rectangle
- `void ellipse(int x, int y, int w, int h)` - Draw an ellipse
- `void triangle(int x1, int y1, int x2, int y2, int x3, int y3)` - Draw a triangle

### Color Control
- `void fill(uint8_t r, uint8_t g, uint8_t b)` - Set the fill color
- `void stroke(uint8_t r, uint8_t g, uint8_t b)` - Set the stroke color
- `void noFill()` - Disable filling
- `void noStroke()` - Disable stroke

### Input
- `int mouseX` - Current mouse X position
- `int mouseY` - Current mouse Y position
- `int mousePressed` - Whether the mouse button is pressed
- `char key` - Last key pressed
- `int keyPressed` - Whether a key is currently pressed
- `int keyIsDown(char k)` - Check if a specific key is pressed

### Math Utilities
- `float map(float value, float start1, float stop1, float start2, float stop2)` - Map a value from one range to another
- `float randomf(float min, float max)` - Generate a random float in a range
- `float constrain(float value, float min, float max)` - Constrain a value to a range
- `float dist(float x1, float y1, float x2, float y2)` - Calculate distance between two points

## Header-Only Usage

p5c can also be used as a single header file. To use it this way:

```c
// Define P5C_IMPLEMENTATION in exactly one source file before including the header
#define P5C_IMPLEMENTATION
#include "include/p5c_single.h"

// Your code here...
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by [p5.js](https://p5js.org/) and [The Coding Train](https://thecodingtrain.com/)
- Created as a learning project for cross-platform C programming
