/**
 * input_test.c - Test mouse and keyboard handling in p5c
 */

#include "../include/p5c.h"
#include <stdio.h>

// Arrow key codes are now defined in p5c.h

// Track the last clicked position
int lastClickX = -1;
int lastClickY = -1;

void setup() {
    // Set the canvas size
    size(640, 480);
    // Set frame rate
    frameRate(60);
}

void draw() {
    // Clear background
    background(40, 40, 40);

    // Draw a circle that follows the mouse
    fill(0, 255, 0);
    stroke(255, 255, 255);
    circle(mouseX, mouseY, 20);

    // If mouse is pressed, draw a red circle
    if (mousePressed) {
        fill(255, 0, 0);
        circle(mouseX, mouseY, 30);
        lastClickX = mouseX;
        lastClickY = mouseY;
    }

    // Draw last clicked position
    if (lastClickX >= 0 && lastClickY >= 0) {
        stroke(255, 255, 0);
        line(lastClickX, lastClickY, mouseX, mouseY);
    }

    // Handle keyboard input
    if (keyIsDown('r')) {
        // Draw red rectangle when 'r' is held
        fill(255, 0, 0);
        rect(50, 50, 100, 100);
    }

    if (keyIsDown('g')) {
        // Draw green rectangle when 'g' is held
        fill(0, 255, 0);
        rect(200, 50, 100, 100);
    }

    if (keyIsDown('b')) {
        // Draw blue rectangle when 'b' is held
        fill(0, 0, 255);
        rect(350, 50, 100, 100);
    }

    // Draw a square that moves with WASD keys only
    static int squareX = 320;
    static int squareY = 240;

    if (keyIsDown('w')) squareY -= 5;
    if (keyIsDown('s')) squareY += 5;
    if (keyIsDown('a')) squareX -= 5;
    if (keyIsDown('d')) squareX += 5;

    fill(255, 255, 255);
    square(squareX, squareY, 50);

    // Draw a circle that moves with arrow keys
    static int circleX = 320;
    static int circleY = 240;

    if (keyIsDown(ARROW_UP)) circleY -= 5;
    if (keyIsDown(ARROW_DOWN)) circleY += 5;
    if (keyIsDown(ARROW_LEFT)) circleX -= 5;
    if (keyIsDown(ARROW_RIGHT)) circleX += 5;

    fill(255, 255, 0);
    circle(circleX, circleY, 30);
}

int main() {
    return run();
}
