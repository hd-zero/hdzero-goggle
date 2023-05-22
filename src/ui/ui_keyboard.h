#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Allocate and setup the virtual keyboard.
 */
void keyboard_init();

/**
 * Returns true if the keyboard is being displayed,
 * otherwise returns false.
 */
bool keyboard_active();

/**
 * Display and set focus on keyboard.
 */
void keyboard_open();

/**
 * Hide keyboard.
 */
void keyboard_close();

/**
 * Scroll through the keyboard keys.
 */
void keyboard_scroll(uint8_t key);

/**
 * Invokes the selected keyboard key.
 */
void keyboard_press();

/**
 * Clears the text buffer.
 */
void keyboard_clear_text();

/**
 * Populates the text buffer.
 */
void keyboard_set_text(const char *text);

/**
 * Populates buffer if keyboard was used and returns
 * the number of characters written into buffer.
 */
int keyboard_get_text(char *buffer, const uint32_t size);
