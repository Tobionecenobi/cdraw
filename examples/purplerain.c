#include "../include/p5c.h"

#define DROP_COUNT 1000
static float speed = 20;

typedef struct drop {
    // Properties
    float x;
    float y;
    float depth;
    float speed;
    float length;
    
    // Methods
    void (*fall)(struct Star* self);
    void (*show)(struct Star* self);
} Drop;

static void drop_fall(Drop* self) {
    self->speed += map(self->depth, 0, 20, 0, 0.2);
    self->y += self->speed;
    self->length = map(self->speed, 0, 5, 10, 20);
    if (self->y > height) {
        self->y = randomf(-height*3, 0);
        self->x = randomf(0, width);
        self->speed = randomf(2, 5);
    }
}

static void drop_show(Drop* self){
    strokeWeight(map(self->depth, 0, 20, 1, 3));
    stroke(138, 43, 226);
    line(self->x, self->y, self->x, self->y + self->length);
}

// Constructor
static Drop* new_drop() {
    Drop* drop = (Drop*)malloc(sizeof(Drop));
    drop->x = randomf(0, width);
    drop->y = randomf(-height*3, 0);
    drop->depth = randomf(0, 20);
    drop->speed = randomf(2, 5);
    drop->fall = drop_fall;
    drop->show = drop_show;
    return drop;
}

static Drop* drops[DROP_COUNT];

void setup() {
    size(640, 360);
    for(int i = 0; i < DROP_COUNT; i++) {
        drops[i] = new_drop();
    }
}

void draw() {
    background(230,230,250);
    for(int i = 0; i < DROP_COUNT; i++) {
        Drop* d = drops[i];
        d->fall(d);
        d->show(d);
    }
}

int main() {
    int result = run();
    
    for(int i = 0; i < DROP_COUNT; i++) {
        free(drops[i]);
    }
    
    return result;
}