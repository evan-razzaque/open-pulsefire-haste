#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <hidapi/hidapi.h>

typedef uint8_t byte;

enum MOUSE_BUTTON {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_SIDE_BACK,
	MOUSE_BUTTON_SIDE_FORWARD,
	MOUSE_BUTTON_DPI
} typedef MOUSE_BUTTON;

/**
 * An enum for mouse actions. Each member is a uint16_t composed of the type of action (mouse, keyboard, etc) and the action itself.
 * Format (in hex bytes): <type> <action>
 */
enum MOUSE_ACTION {
    DISABLED     = 0x0000,
    
    LEFT_CLICK   = 0x0101,
    RIGHT_CLICK  = 0x0102,
    MIDDLE_CLICK = 0x0103,
    BACK         = 0x0104,
    FORWARD      = 0x0105,
    DPI_SWITCH   = 0x0708,

    KEY_A        = 0x0204, // TODO: add special keys

    PLAY_PAUSE   = 0x0300,
    STOP         = 0x0301,
    PREVIOUS     = 0x0302,
    NEXT         = 0x0303,
    MUTE         = 0x0304,
    VOLUME_DOWN  = 0x0305,
    VOLUME_UP    = 0x0306
    
} typedef MOUSE_ACTION;

/**
 * Change a binding for a mouse button.
 * 
 * @param dev The mouse device handle
 * @param options The mouse button to re-assign
 * @param action The action to assign to the button
 * @return the number of bytes written or -1 on error
 */
int assign_button(hid_device *dev, MOUSE_BUTTON button, MOUSE_ACTION action);

#endif