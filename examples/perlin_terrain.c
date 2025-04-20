/**
 * perlin_terrain.c - Implementation of Perlin Noise Terrain from The Coding Train
 *
 * This example creates a simple 2D terrain using a 1D Perlin noise implementation.
 */

#include "../include/p5c.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Simplified Perlin noise implementation
// This is a very basic implementation for demonstration purposes
float noise(float x) {
    // Convert to integer grid coordinates
    int xi = (int)x & 255;

    // Get fractional part
    float xf = x - (int)x;

    // Hash function to generate pseudo-random gradients
    int h = (xi * 16807) % 259;

    // Generate value
    float n = sin(h * 0.01745f); // Convert degrees to radians (h * PI/180)

    return n * 0.5f + 0.5f; // Map to 0-1 range
}

// Variables for the terrain
float xoff = 0.0f;
float yoff = 0.0f;
float inc = 0.01f;
int cols, rows;
int scl = 20;
int w = 2000;
int h = 1600;

void setup() {
    // Set the canvas size
    size(640, 480);

    // Calculate columns and rows
    cols = w / scl;
    rows = h / scl;

    // Set frame rate
    frameRate(30);
}

void draw() {
    // Set the background color
    background(0, 0, 0);

    // Start with a new offset for each frame to animate
    float xoff = frameCount * 0.01f;

    // Draw the terrain
    stroke(255, 255, 255);
    noFill();

    // Draw multiple lines to create a terrain effect
    for (int y = 0; y < 5; y++) {
        float yoff = y * 0.2f;

        // Begin shape
        int prevX = 0;
        int prevY = height / 2 + (int)(noise(xoff + yoff) * 200) - 100;

        for (int x = 0; x < width; x += 5) {
            // Get noise value
            float n = noise(xoff + x * 0.005f + yoff);

            // Map noise to screen coordinates
            int yPos = height / 2 + (int)(n * 200) - 100 + y * 30;

            // Draw line segment
            line(prevX, prevY, x, yPos);

            // Update previous point
            prevX = x;
            prevY = yPos;
        }
    }

    // Draw some "stars" in the background
    for (int i = 0; i < 100; i++) {
        int x = (int)randomf(0, width);
        int y = (int)randomf(0, height / 2);
        point(x, y);
    }

    // Draw a "moon"
    fill(200, 200, 200);
    noStroke();
    ellipse(100, 100, 50, 50);

    // Draw some mountains in the foreground
    fill(100, 100, 100);
    for (int i = 0; i < width; i += 200) {
        int mHeight = (int)(150 + noise(i * 0.01f) * 100);
        triangle(
            i, height,
            i + 100, height - mHeight,
            i + 200, height
        );
    }
}

int main() {
    return run();
}
