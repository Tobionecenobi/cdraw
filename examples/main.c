/**
 * main.c - Simple example for the p5c drawing library
 */

#include "../include/p5c.h"
#include <stdio.h>
#include <stdlib.h>

// Global variables for animation
float x, y;
float xspeed = 2.5;
float yspeed = 2;
int radius = 20;

// Setup function - called once at the beginning
void setup() {
    printf("Setup called\n");

    // Set the canvas size
    size(640, 480);

    // Initialize position
    x = width / 2;
    y = height / 2;

    // Set frame rate
    frameRate(60);

    printf("Setup completed\n");
}

// Draw function - called every frame
void draw() {
    static int frame_counter = 0;

    if (frame_counter == 0) {
        printf("First draw call\n");
    }

    if (frame_counter % 60 == 0) {
        printf("Draw frame %d\n", frame_counter);
    }

    frame_counter++;

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

    // Draw a rectangle that follows the mouse
    fill(0, 255, 0);
    square(mouseX - 25, mouseY - 25, 50);

    // Draw a line from the circle to the mouse
    stroke(255, 255, 0);
    line(x, y, mouseX, mouseY);
}

// Main function
int main() {
    printf("Starting main\n");

    // Run the sketch
    int result = run();

    printf("Exiting with code %d\n", result);
    return result;
}
