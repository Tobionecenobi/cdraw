/**
 * test_pattern.c - Test pattern using all primitives
 */

#include "../include/p5c.h"
#include <stdlib.h>
#include <stdio.h>

void setup() {
    // Set the canvas size
    size(640, 480);

    // Set frame rate
    frameRate(60);
}

void draw() {
    // Set the background color
    background(40, 40, 40);

    // Draw a grid of points
    stroke(200, 200, 200);
    for (int x = 0; x < width; x += 20) {
        for (int y = 0; y < height; y += 20) {
            point(x, y);
        }
    }

    // Draw some lines
    stroke(255, 0, 0); // Red
    for (int i = 0; i < 10; i++) {
        line(0, i * 30, width, height - i * 30);
    }

    // Draw rectangles
    fill(0, 255, 0); // Green
    stroke(255, 255, 255); // White
    for (int i = 0; i < 5; i++) {
        rect(50 + i * 30, 50 + i * 30, 100, 100);
    }

    // Draw ellipses
    fill(0, 0, 255); // Blue
    stroke(255, 255, 0); // Yellow
    for (int i = 0; i < 5; i++) {
        ellipse(400 + i * 20, 100 + i * 20, 80, 80);
    }

    // Draw triangles
    fill(255, 0, 255); // Magenta
    stroke(0, 255, 255); // Cyan
    for (int i = 0; i < 3; i++) {
        triangle(
            400 + i * 30, 300 + i * 30,
            450 + i * 30, 400 + i * 30,
            350 + i * 30, 400 + i * 30
        );
    }

    // Draw a shape with no fill
    noFill();
    stroke(255, 128, 0); // Orange
    rect(600, 100, 150, 150);

    // Draw a shape with no stroke
    fill(128, 0, 255); // Purple
    noStroke();
    ellipse(650, 350, 100, 100);

    // Draw text showing mouse position
    fill(255, 255, 255);
    stroke(0, 0, 0);

    // We can't draw text yet, so draw a rectangle at mouse position
    rect(mouseX - 5, mouseY - 5, 10, 10);

    // Draw a line from center to mouse
    stroke(255, 255, 255);
    line(width / 2, height / 2, mouseX, mouseY);
}

int main() {
    return run();
}
