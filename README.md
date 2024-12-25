# RaylibIntendedLibrary
This is a library that is an extension of the RayLib game development library.

> Because Raylib is great, but boilerplate isn't.

So far, this library only has a button definition, but I plan on adding much more, as long as there is demand for this.
## The overarching goal of this library
Speed up the—admittedly already quite fast—GUI development in RayLib.
The goal is to have the simplicity of RayLib with some pre-made patterns.
## Example usage
First, define the state type.
```C
#include "raylib.h"
#include "ril.h"
// inside main.c
typedef struct State {} State;

#define STATE_TYPE State*
// ...
int main() {
    // ... Raylib code
    State* state = malloc(sizeof(State));
    // ... Other Raylib code
}
```
And whenever you want to construct an object, pass the state around like so.
```C
    struct ril_ColorButton button = ril_CreateColorButton(250, 300, 300, 100, ORANGE);
    ril_ButtonOnClick(&(button.innerButton), changeToGame);
    ril_RenderColorButtonWithText(button, "New Game", BLACK, state);
```
You can use the State struct for whatever information you want to persist across frames.
I should note that all of the render functions(e.g. ril_RenderColorButton), should be between
`BeginDrawing()` and `EndDrawing()`.
Since the state lasts for as long as the program runs, you technically don't need to deallocate it.
## A non-comprehensive to-do list
- [ ] All button types:
  - [ ] Textureless button
  - [ ] A colored button
  - [ ] Button with a 2DTexture
- [ ] 3D Render handling
  - [ ] A 3D world struct(containing the player(camera), objects(planes, cubes, ...), ...)
