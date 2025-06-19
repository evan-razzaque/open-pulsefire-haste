#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk-4.0/gdk/gdkkeysyms.h>

typedef uint8_t byte;

enum MOUSE_BUTTON {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_SIDE_BACK,
	MOUSE_BUTTON_SIDE_FORWARD,
	MOUSE_BUTTON_DPI_TOGGLE
} typedef MOUSE_BUTTON;

/**
 * An enum for simple mouse actions (mouse, media, and windows shortcut).
 * Each member is a uint16_t composed of the type of action and the action itself.
 * Format (in hex bytes): <type> <action>
 */
enum SIMPLE_MOUSE_ACTION {
    DISABLED       = 0x0000,
    
    LEFT_CLICK     = 0x0101,
    RIGHT_CLICK    = 0x0102,
    MIDDLE_CLICK   = 0x0103,
    BACK           = 0x0104,
    FORWARD        = 0x0105,
    DPI_TOGGLE     = 0x0708,

    PLAY_PAUSE     = 0x0300,
    STOP           = 0x0301,
    PREVIOUS       = 0x0302,
    NEXT           = 0x0303,
    MUTE           = 0x0304,
    VOLUME_DOWN    = 0x0305,
    VOLUME_UP      = 0x0306,
    
    TASK_MANAGER   = 0x0501,
    SYSTEM_UTILITY = 0x0502,
    SHOW_DESKTOP   = 0x0503,
    CYCLE_APPS     = 0x0504,
    CLOSE_APPS     = 0x0505,
    CUT            = 0x0506,
    COPY           = 0x0507,
    PASTE          = 0x0508,

} typedef SIMPLE_MOUSE_ACTION;

enum MACRO_EVENT_TYPE {
    MACRO_EVENT_TYPE_KEYBOARD = 0x1a,
    MACRO_EVENT_TYPE_MOUSE    = 0x25
} typedef MACRO_EVENT_TYPE;

enum MACRO_REPEAT_MODE {
    MACRO_REPEAT_MODE_ONCE = 0x00,
    MACRO_REPEAT_MODE_TOGGLE_REPEAT = 0x02,
    MACRO_REPEAT_MODE_HOLD_REPEAT = 0x03,
} typedef MACRO_REPEAT_MODE;

struct macro_key_event {
    byte modifier_keys;
    byte keys[5];
    uint16_t delay_next_action;
} typedef macro_key_event;

/**
 * Change a binding for a mouse button.
 * 
 * @param dev The mouse device handle
 * @param options The mouse button to re-assign
 * @param action The action to assign to the button
 * @return the number of bytes written or -1 on error
 */
int assign_button_action(hid_device *dev, MOUSE_BUTTON button, uint16_t action);

/**
 * Assign a macro to a mouse button.
 * 
 * @param dev The mouse device handle
 * @param options The mouse button to re-assign
 * @param action The action to assign to the button
 * @return the number of bytes written or -1 on error
 */
int assign_button_macro(hid_device *dev, MOUSE_BUTTON button);

#endif
