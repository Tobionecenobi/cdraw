/**
 * arc_test.c - Test the arc drawing function in p5c
 */

#include "../include/p5c.h"
#include <stdio.h>

void setup() {
    // Set the canvas size
    size(640, 480);
    // Set frame rate
    frameRate(30);
    // Use degrees for angles
    angleMode(DEGREES);
}

void draw() {
    // Clear background
    background(40, 40, 40);
    
    // Draw arcs with different modes
    
    // OPEN arc (default)
    fill(255, 0, 0);
    stroke(255, 255, 255);
    arc(100, 100, 150, 150, 0, 135);
    
    // CHORD arc
    fill(0, 255, 0);
    stroke(255, 255, 255);
    arcMode(320, 100, 150, 150, 0, 135, CHORD);
    
    // PIE arc
    fill(0, 0, 255);
    stroke(255, 255, 255);
    arcMode(100, 300, 150, 150, 0, 135, PIE);
    
    // Arc with custom detail
    fill(255, 255, 0);
    stroke(255, 255, 255);
    arcDetail(320, 300, 150, 150, 0, 135, PIE, 10);
    
    // Draw text to label the arcs
    fill(255, 255, 255);
    // Note: p5c doesn't have text drawing, so we'll use shapes to indicate the modes
    
    // OPEN indicator (just a line)
    line(100, 50, 150, 50);
    
    // CHORD indicator (line with endpoints connected)
    line(320, 50, 370, 50);
    line(320, 50, 370, 60);
    line(370, 50, 370, 60);
    
    // PIE indicator (triangle)
    triangle(100, 250, 150, 250, 125, 270);
    
    // Custom detail indicator (circle with fewer segments)
    circle(320, 250, 10);
}

int main() {
    return run();
}
