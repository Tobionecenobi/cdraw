/**
 * random_walker.c - Implementation of the Random Walker from The Coding Train
 */

#include "../include/p5c.h"
#include <stdlib.h>
#include <stdio.h>

// Walker position
int walkerX, walkerY;

void setup() {
    printf("Walker setup called\n");

    // Set the canvas size
    size(640, 480);

    // Start in the middle
    walkerX = width / 2;
    walkerY = height / 2;

    // Set frame rate
    frameRate(30);

    printf("Walker setup completed\n");
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
    printf("Starting walker main\n");

    // Run the sketch
    int result = run();

    printf("Exiting walker with code %d\n", result);
    return result;
}
