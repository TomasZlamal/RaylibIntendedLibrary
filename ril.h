/*
 * Author: Tomáš Zlámal
 * License: Apache License 2.0
 * Raylib Intended Library
 */

/* User Guide
 * The way this library works is that it passed around a state struct.
 * This state struct can be anything you want it to be, but you need to define it.
 * Define it via the STATE_TYPE macro. Input a pointer to your state struct.
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

//
// Base Button Functions and Structs
//
// 


typedef struct ril_Button {
    Rectangle rectangle; // bounds
    void (*cb)(STATE_TYPE state); // callback
    void (*on_hover)(STATE_TYPE state); // on_hover callback
    opt_sound sound; // optional sound
} ril_Button;

// 
// Button Factory Methods
//

ril_Button ril_CreateButton(int x, int y, int w, int h) {
    Rectangle rectangle = {x, y, w, h};
    opt_sound opt_s;
    opt_s.present = false;
    ril_Button button = {rectangle, 0, 0, opt_s};
    return button;
}
void ril_ButtonAddSound(ril_Button* btn, Sound _sound) {
    btn->sound.present = 1;
    btn->sound.value = _sound;
}

// note that this function will be called every frame the user hovers on the button
void ril_ButtonOnHover(ril_Button* btn, void (*on_hover)(STATE_TYPE state)) {
    btn->on_hover = on_hover;
}
void ril_ButtonOnClick(ril_Button* btn, void (*cb)(STATE_TYPE state)) {
    btn->cb = cb;
}

//
// Button render functions
//

void ril_RenderButton(ril_Button button, STATE_TYPE state) {
    Rectangle rectangle = button.rectangle;
    if(CheckCollisionPointRec(GetMousePosition(), rectangle)) {
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if(button.sound.present) {
                PlaySound(button.sound.value);
            }
            if(button.cb) {
                button.cb(state);
            }
        }
        else if(button.on_hover) {
            button.on_hover(state);
        }
    }
}


void ril_RenderButtonWithText(ril_Button button, const char* textContent, Color textColor, STATE_TYPE state) {
    ril_RenderButton(button, state);

    Rectangle rectangle = button.rectangle;
    int charCount = strnlen(textContent, MAX_BUTTON_TEXT_LENGTH);
    int textX, textY, textSize;
    textSize = rectangle.height/4;
    textY = rectangle.y + textSize*1.5;
    // approximation of width to height of the default font
    textX = rectangle.x + (rectangle.width - charCount*textSize*FONT_WIDTH_TO_HEIGHT_RATIO) / 2;
     
    DrawText(textContent, textX, textY, textSize, textColor);
}

//
// Button with a colored background
//
//

typedef struct ril_ColorButton {
    ril_Button inner_btn;
    Color backgroundColor;
} ril_ColorButton;


//
// ColorButton Factory methods
//

ril_ColorButton ril_CreateColorButton(int x, int y, int w, int h, Color color) {
    ril_Button btn = ril_CreateButton(x, y, w, h);
    ril_ColorButton out = {btn, color};
    return out;
}

//
// ColorButton Render methods
//

void ril_RenderColorButton(ril_ColorButton button, STATE_TYPE state) {
    Rectangle rectangle = button.inner_btn.rectangle;
    DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, button.backgroundColor);
    ril_RenderButton(button.inner_btn, state); 
}

void ril_RenderColorButtonWithText(ril_ColorButton button, const char* textContent, Color textColor, STATE_TYPE state) {
    Rectangle rectangle = button.inner_btn.rectangle;
    DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, button.backgroundColor);

    ril_RenderButtonWithText(button.inner_btn, textContent, textColor, state);
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
