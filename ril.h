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

#include <stdio.h>

//
// Util Definitions
//
//

// C-like implementation of the optional type
#define optional(type) struct { bool present; type value; }
typedef optional(Sound) opt_sound;

#define MAX_BUTTON_TEXT_LENGTH 200
#define FONT_WIDTH_TO_HEIGHT_RATIO 4/7
#if defined(__GNUC__) || defined(__clang__)
    #define DYNAMIC_ALLOC __attribute__((warn_unused_result))
#else
    #define DYNAMIC_ALLOC
#endif

int ril_FrameCount = 0;

void ril_IntToChar(char* buf, int i) {
    sprintf(buf, "%d", i);
}
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
    textX = rectangle.x + (rectangle.width - charCount*textSize*FONT_WIDTH_TO_HEIGHT_RATIO) / 2+4;
     
    DrawText(textContent, textX, textY, textSize, textColor);
}

//
// Button with a colored background
//
//

typedef struct ril_ColorButton {
    ril_Button innerButton;
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
    Rectangle rectangle = button.innerButton.rectangle;
    DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, button.backgroundColor);
    ril_RenderButton(button.innerButton, state); 
}

void ril_RenderColorButtonWithText(ril_ColorButton button, const char* textContent, Color textColor, STATE_TYPE state) {
    Rectangle rectangle = button.innerButton.rectangle;
    DrawRectangle(rectangle.x, rectangle.y, rectangle.width, rectangle.height, button.backgroundColor);

    ril_RenderButtonWithText(button.innerButton, textContent, textColor, state);
}


// 
// Button with image
//
//

typedef struct ril_SpriteButton {
    ril_Button innerButton;
    Texture2D texture;
    Color tint;
} ril_SpriteButton;


ril_SpriteButton ril_CreateSpriteButton(int x, int y, Texture2D texture, Color tint) {
    ril_Button btn = ril_CreateButton(x, y, texture.width, texture.height);
    ril_SpriteButton out = {btn, texture, tint};
    return out;
}

void ril_RenderSpriteButton(ril_SpriteButton button, STATE_TYPE state) {
    ril_RenderButton(button.innerButton, state);
    DrawTexture(button.texture, button.innerButton.rectangle.x, button.innerButton.rectangle.y, button.tint);
}

// 
// 2D Point Graphs
//
//

typedef struct ril_Graph2D {
    Vector2* points; // sorted
    int len;
    Rectangle bounds;
    int min_y;
    int max_y;
    Color line_color;
} ril_Graph2D;

// VERY IMPORTANT: this function does NOT take ownership of points
ril_Graph2D ril_CreateGraph2D(Vector2* points, int len, Rectangle bounds, Color color) {
    
    if(len == 0) {
        ril_Graph2D out = {0, 0, bounds, 0, 0};
        return out;
    }

    // sort the graph
    int min_y = points[0].y;
    int max_y = points[0].y;
    for(int i = 0; i < len; i++) {
        for(int j = i+1; j < len; j++) {
            if(points[j].x < points[i].x) {
                Vector2 temp = points[j];
                points[j] = points[i];
                points[i] = temp;
            }
        }
        if(points[i].y > max_y) {
            max_y = points[i].y;
        }
        if(points[i].y < min_y) {
            min_y = points[i].y;
        }
    }
    ril_Graph2D out = {points, len, bounds, min_y, max_y, color};
    return out;
}

void ril_DrawGraph2D(ril_Graph2D graph) {
    int offset_x = graph.bounds.x;
    int offset_y = graph.bounds.y;
    int multiplier_x = graph.bounds.width / (graph.points[graph.len-1].x - graph.points[0].x);
    int multiplier_y = graph.bounds.height / (graph.max_y - graph.min_y);
    for(int i = 0; i < graph.len-1; i++) {
        Vector2* pointA = graph.points+i;
        Vector2* pointB = graph.points+i+1;
        DrawLine(
                pointA->x*multiplier_x+offset_x,
                pointA->y*multiplier_y+offset_y,
                pointB->x*multiplier_x+offset_x,
                pointB->y*multiplier_y+offset_y,
                graph.line_color
                );
    }
}

// 
// Text Input
//
// 

typedef struct ril_TextInput {
    // With base methods
    Rectangle bounds;
    const char* prompt_text;
    const char* input_label;

    // With other factory methods
    char* buf;
    int max_len;
    int curr_len;
    void(*cb)(STATE_TYPE state);
} ril_TextInput;

ril_TextInput ril_CreateTextInput(int x, int y, int w, int h, const char* prompt, const char* label) {
    Rectangle rec = {x, y, w, h};
    ril_TextInput text = {rec, prompt, label, 0, 0, 0, 0};
    return text;
}

void ril_SetTextInputBuffer(ril_TextInput* in, char* buf, int max) {
    in->buf = buf;
    buf[0] = '\0';
    in->max_len = max;
}
void ril_SetTextInputCallback(ril_TextInput* input, void(*cb)(STATE_TYPE state)) {
    input->cb = cb;
}
void ril_DrawTextInput(ril_TextInput* input, STATE_TYPE state) {
    Rectangle textBox = input->bounds;
    bool mouseOnText = CheckCollisionPointRec(GetMousePosition(), textBox);

    //
    // INPUT DETECTION
    //
    if (mouseOnText)
    {
        // Set the window's cursor to the I-Beam
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0)
        {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (input->curr_len < input->max_len))
            {
                input->buf[input->curr_len] = (char)key;
                input->buf[input->curr_len+1] = '\0'; // Add null terminator at the end of the string.
                input->curr_len++;
            }

            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            input->curr_len--;
            if (input->curr_len < 0) input->curr_len = 0;
            input->buf[input->curr_len] = '\0';
        }
    }
    else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    // drawing
    //
    DrawText(input->prompt_text, textBox.x, textBox.y-20, 20, GRAY);
    DrawRectangleRec(textBox, LIGHTGRAY);
    if (mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
    else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(input->buf, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

    DrawText(TextFormat("%i/%i", input->curr_len, input->max_len), textBox.x+textBox.width+10, textBox.y, 20, DARKGRAY);

    if (mouseOnText)
    {
        if (input->curr_len < input->max_len)
        {
            // Draw blinking underscore char
            if (((ril_FrameCount/20)%2) == 0) DrawText("_", (int)textBox.x + 8 + MeasureText(input->buf, 40), (int)textBox.y + 12, 40, MAROON);
        }
    }
}
#endif
