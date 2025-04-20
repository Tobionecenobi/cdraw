/**
 * header_only_example.c - Example of using the header-only version of p5c
 */

// Define P5C_IMPLEMENTATION before including the header
#define P5C_IMPLEMENTATION
#include "../include/p5c_single.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Global variables
float angle = 0.0f;

void setup() {
    // Set the canvas size
    size(640, 480);

    // Set frame rate
    frameRate(60);
}

void draw() {
    // Set the background color
    background(40, 40, 40);

    // Calculate center of screen
    int centerX = width / 2;
    int centerY = height / 2;

    // Draw a rotating square
    fill(255, 0, 0);
    stroke(255, 255, 255);

    // Calculate the four corners of a square that rotates around the center
    float size = 100;
    float halfSize = size / 2;

    // Rotate the angle
    angle += 0.02f;

    // Calculate the four corners with rotation
    int x1 = centerX + (int)(cos(angle) * halfSize - sin(angle) * halfSize);
    int y1 = centerY + (int)(sin(angle) * halfSize + cos(angle) * halfSize);

    int x2 = centerX + (int)(cos(angle) * halfSize - sin(angle) * -halfSize);
    int y2 = centerY + (int)(sin(angle) * halfSize + cos(angle) * -halfSize);

    int x3 = centerX + (int)(cos(angle) * -halfSize - sin(angle) * -halfSize);
    int y3 = centerY + (int)(sin(angle) * -halfSize + cos(angle) * -halfSize);

    int x4 = centerX + (int)(cos(angle) * -halfSize - sin(angle) * halfSize);
    int y4 = centerY + (int)(sin(angle) * -halfSize + cos(angle) * halfSize);

    // Draw the square as two triangles
    triangle(x1, y1, x2, y2, x3, y3);
    triangle(x1, y1, x3, y3, x4, y4);

    // Draw a circle that follows the mouse
    fill(0, 255, 0);
    ellipse(mouseX - 20, mouseY - 20, 40, 40);

    // Draw text showing frame count (as a rectangle since we don't have text)
    fill(255, 255, 255);
    rect(10, 10, frameCount % 100, 10);

    // Check for key presses
    if (keyIsDown('r')) {
        // Reset angle when 'r' is pressed
        angle = 0.0f;
    }
}

int main() {
    return run();
}
