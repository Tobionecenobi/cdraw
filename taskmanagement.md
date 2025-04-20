## 📁 Phase 1: Project Skeleton and Window Creation

### 1.1 Create File Structure
- [x] Create folders: `src/`, `include/`, `examples/`, `build/`
- [x] Create empty files: `main.c`, `p5c.c`, `p5c.h`, `Makefile`

### 1.2 Basic Project Initialization
- [x] Add a minimal `main.c` with empty `setup()` and `draw()` functions
- [x] Create a compile-able no-op loop (`while(1)`)

### 1.3 Cross-Platform Window Initialization
#### Windows
- [x] Create WinMain entry point
- [x] Register a window class
- [x] Create a window with `CreateWindowEx`
- [x] Show and update the window
- [x] Handle `WM_CLOSE`, `WM_DESTROY`
- [x] Implement a Windows message loop (`GetMessage`, `TranslateMessage`, `DispatchMessage`)

#### Linux
- [x] Open display with `XOpenDisplay`
- [x] Create window using `XCreateSimpleWindow`
- [x] Select input events
- [x] Map and show window
- [x] Implement X11 event loop (`XNextEvent`, handle `Expose`, `KeyPress`, `ClientMessage`)

---

## 🖍️ Phase 2: Pixel Drawing & Buffers

### 2.1 Framebuffer or Bitmap Buffers
- [x] Allocate an off-screen pixel buffer (`uint32_t* framebuffer`)
- [x] Create a function to clear the framebuffer
- [x] Write a function to set a pixel: `setPixel(x, y, r, g, b)`
- [x] Copy framebuffer to screen on each frame

#### Windows
- [x] Use `StretchDIBits()` with `BITMAPINFO` to render the framebuffer

#### Linux
- [x] Use `XPutImage()` with `XImage` to render the framebuffer

---

## 🧱 Phase 3: Core Primitives

- [x] `void point(int x, int y)`
- [x] `void line(int x1, int y1, int x2, int y2)` — use Bresenham's line algorithm
- [x] `void rect(int x, int y, int w, int h)`
- [x] `void ellipse(int x, int y, int w, int h)` — use midpoint ellipse algorithm
- [x] `void triangle(int x1, int y1, int x2, int y2, int x3, int y3)` — draw filled triangle or outline

---

## 🎨 Phase 4: Fill and Stroke Control

- [x] Define global color variables: `fillColor`, `strokeColor`
- [x] `void fill(r, g, b)`
- [x] `void stroke(r, g, b)`
- [x] `void noFill()`
- [x] `void noStroke()`
- [x] Modify primitives to respect fill/stroke flags

---

## 🌀 Phase 5: Setup/Draw/Loop

- [x] Create internal `_setup()` and `_draw()` function pointers
- [x] Call `setup()` once before the loop
- [x] Call `draw()` every frame
- [x] Add `frameRate(int fps)` function
- [x] Implement `delay()` or `sleep()` logic to control FPS
- [x] Add `frameCount` variable

---

## 🖱️ Phase 6: Input System

### Mouse Input
- [x] Track `mouseX`, `mouseY`
- [x] Set `mousePressed` flag on button down
- [x] Set `mouseReleased` on button up

### Keyboard Input
- [x] Capture keypresses and store current `key` value
- [x] Set `keyPressed` / `keyReleased` flags
- [x] Add `keyIsDown(char)` API

---

## ➗ Phase 7: Math Utilities

- [x] `float map(float val, float inMin, float inMax, float outMin, float outMax)`
- [x] `float random(float min, float max)`
- [x] `float constrain(float val, float min, float max)`
- [x] `float dist(float x1, float y1, float x2, float y2)`

---

## 📦 Phase 8: Cleanup and API Design

- [x] Move all user-exposed functions to `p5c.h`
- [x] Wrap platform-specific code in `#ifdef _WIN32` / `#ifdef __linux__`
- [x] Define types like `Color`, `Vec2` if needed
- [x] Document each API function inline
- [x] Ensure every drawing call uses only internal pixel buffer

---

## 🧪 Phase 9: Examples and Testing

- [x] Create a sketch for a bouncing ball
- [x] Create a sketch using mouse interaction
- [x] Recreate "random walker" from Coding Train
- [x] Recreate "perlin noise terrain"
- [x] Make a test pattern that uses all primitives

---

## 🛠️ Phase 10: Build System and Docs

- [x] Create `Makefile` for Linux with gcc
- [x] Create `Makefile` or `.bat` for Windows with MinGW
- [x] Write `README.md`:
  - [x] Description
  - [x] How to build on Linux/Windows
  - [x] API usage example
  - [x] License (e.g., MIT)
- [x] Optional: create `.h` only version for header-only usage

---

## 🎉 All tasks completed!

The cross-platform drawing engine is now complete and working on both Windows and Linux platforms. All examples are running successfully.
