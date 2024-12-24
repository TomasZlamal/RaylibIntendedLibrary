/*
 * Author: Tomáš Zlámal
 * License: Apache License 2.0
 * Raylib Intended Library
 */

#ifndef RAYLIB_INTENDED_LIBRARY_H
#define RAYLIB_INTENDED_LIBRARY_H

#ifndef RAYLIB_H
#warning "Raylib not detected or corrupted"
#endif

//
// Util Definitions
//
//

// C-like implementation of the optional type
#define optional(type) struct { bool present; type value; }
typedef optional(Sound) opt_sound;

#define MAX_BUTTON_TEXT_LENGTH 200
#define FONT_WIDTH_TO_HEIGHT_RATIO 4/7


#ifdef STATE_TYPE
    // State is handled via passing of a struct
    #define EXPANDED_TYPE STATE_TYPE* state
#else
    // State is either handled:
    // 1. Via global variables
    // 2. Not at all
    #define EXPANDED_TYPE
#endif

//
// Base Button Functions and Structs
//
// 

typedef struct ril_Button {
    Color backgroundColor;
    Rectangle rectangle; // bounds
    void (*cb)(EXPANDED_TYPE); // callback
    void (*on_hover)(EXPANDED_TYPE); // on_hover callback
    opt_sound sound; // optional sound
} ril_Button;

// 
// Button Factory Methods
//

ril_Button ril_CreateButton(int x, int y, int w, int h, Color color) {
    Rectangle rectangle = {x, y, w, h};
    opt_sound opt_s;
    opt_s.present = false;
    ril_Button button = {color, rectangle, 0, 0, opt_s};
    return button;
}
void ril_ButtonAddSound(ril_Button* btn, Sound _sound) {
    btn->sound.present = 1;
    btn->sound.value = _sound;
}

// note that this function will be called every frame the user hovers on the button
void ril_ButtonOnHover(ril_Button* btn, void (*on_hover)(EXPANDED_TYPE)) {
    btn->on_hover = on_hover;
}
void ril_ButtonOnClick(ril_Button* btn, void (*cb)(EXPANDED_TYPE)) {
    btn->cb = cb;
}

//
// Button render functions
//

#ifdef STATE_TYPE
void ril_DrawButton(ril_Button button, EXPANDED_TYPE) {
#else
void ril_DrawButton(ril_Button button) {
#endif
    Rectangle rectangle = button.rectangle;
    DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, button.backgroundColor);
    if(CheckCollisionPointRec(GetMousePosition(), rectangle)) {
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if(button.sound.present) {
                PlaySound(button.sound.value);
            }
            if(button.cb) {
#ifdef STATE_TYPE
                button.cb(state);
#else
                button.cb();
#endif
            }
        }
#ifdef STATE_TYPE
            if(button.on_hover) {
                button.on_hover(state);
#else
                button.on_hover();
#endif
            }
    }
}

/*
 * drawButtonWithText();
 * The two options for the text from the perspective of the user are content and color
 * The size and location are in proportion to the size of the button
 */

#ifdef STATE_TYPE
void ril_DrawButtonWithText(ril_Button button, const char* textContent, Color textColor, EXPANDED_TYPE) {
    ril_DrawButton(button, state);
#else
void ril_DrawButtonWithText(ril_Button button, const char* textContent, Color textColor) {
    ril_DrawButton(button);
#endif

    Rectangle rectangle = button.rectangle;
    int charCount = strnlen(textContent, MAX_BUTTON_TEXT_LENGTH);
    int textX, textY, textSize;
    textSize = rectangle.height/4;
    textY = rectangle.y + textSize*1.5;
    // 4/7 is an approximation of width to height of the default font
    textX = rectangle.x + (rectangle.width - charCount*textSize*FONT_WIDTH_TO_HEIGHT_RATIO) / 2;
     
    DrawText(textContent, textX, textY, textSize, textColor);
}

// 
// Button with image
//
//

// Technically this is inefficient because ril_Button has a redundant Color field
// A way to solve this would be to create another struct, TexturelessButton,
// which would be the new base class that all button structs have
// this new struct wouldn't have a Color field.
// TODO: Divide render functionality between texture-less buttons
typedef struct ril_SpriteButton {
    ril_Button button;
    Texture2D texture;
} ril_SpriteButton;
#endif
