#include "../include/p5c.h"
#include <stdio.h>
#include <stdlib.h>

#define STAR_COUNT 800
static float speed = 20;

typedef struct Star {
    // Properties
    float x;
    float y;
    float z;
    float pz;
    
    // Methods
    void (*update)(struct Star* self);
    void (*show)(struct Star* self);
} Star;

// Method implementations
static void star_update(Star* self) {
    self->z = self->z - speed;
    if (self->z < 1) {
        self->z = width;
        self->x = randomf(-width/2, width/2);
        self->y = randomf(-height/2, height/2);
        self->pz = self->z;
    }
}

static void star_show(Star* self) {
    fill(255, 255, 255);
    noStroke();

    float sx = map(self->x / self->z, 0, 1, 0, width);
    float sy = map(self->y / self->z, 0, 1, 0, height);

    float r = map(self->z, 0, width, 16, 0);
    ellipse(sx - r/2, sy - r/2, r, r);

    float px = map(self->x / self->pz, 0, 1, 0, width);
    float py = map(self->y / self->pz, 0, 1, 0, height);

    stroke(255, 255, 255);
    line(px, py, sx, sy);

    self->pz = self->z;
}

// Constructor
static Star* new_star() {
    Star* star = (Star*)malloc(sizeof(Star));
    star->x = randomf(-width/2, width/2);
    star->y = randomf(-height/2, height/2);
    star->z = randomf(0, width);
    star->pz = star->z;
    star->update = star_update;
    star->show = star_show;
    return star;
}

// Global array of star pointers
static Star* stars[STAR_COUNT];

void setup() {
    size(800, 800);
    frameRate(60);
    
    // Create all stars
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i] = new_star();
    }
}

void draw() {
    background(30, 30, 30);
    
    push();
    translate(width/2, height/2);
    
    // Update and show all stars
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i]->update(stars[i]);  // Method call syntax with ->
        stars[i]->show(stars[i]);
    }
    
    pop();
}

int main() {
    int result = run();
    
    // Cleanup
    for (int i = 0; i < STAR_COUNT; i++) {
        free(stars[i]);
    }
    
    return result;
}