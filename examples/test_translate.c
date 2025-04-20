#include "../include/p5c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Setup function - called once at the beginning
void setup() {
    printf("Setup called\n");

    // Set the canvas size
    size(640, 480);

    // Set frame rate
    frameRate(30);

    printf("Setup completed\n");
}

void draw() {
    background(200, 200, 200);
    
    // Draw a red square at original position
    fill(255, 0, 0);
    square(0, 0, 20);
    
    // Save current transform
    push();
    
    // Move again (accumulates with previous translation)
    translate(width / 2, height / 2);
    
    // Draw a green square at twice-translated position
    fill(0, 255, 0);
    square(0, 0, 20);
    
    // Restore original transform
    pop();

    // Draw a blue square at original position
    fill(0, 0, 255);
    square(0, 0, 10);
}

// Main function
int main() {
    printf("Starting main\n");

    // Run the sketch
    int result = run();

    printf("Exiting with code %d\n", result);
    return result;
}