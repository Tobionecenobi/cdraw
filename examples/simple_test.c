/**
 * simple_test.c - A very simple test for the p5c library
 */

#include "../include/p5c.h"

void setup() {
    // Set the canvas size
    size(640, 480);
    
    // Set frame rate
    frameRate(30);
}

void draw() {
    // Set the background color
    background(0, 0, 0);
    
    // Draw a rectangle in the center
    fill(255, 0, 0);
    stroke(255, 255, 255);
    rect(width/2 - 50, height/2 - 50, 100, 100);
    
    // Draw a circle that follows the mouse
    fill(0, 255, 0);
    ellipse(mouseX - 25, mouseY - 25, 50, 50);
}

int main() {
    return run();
}
