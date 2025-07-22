#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk-4.0/gdk/gdkkeysyms.h>

typedef uint8_t byte;

enum {
    MOUSE_ACTION_TYPE_KEYBOARD = 0x02,
    MOUSE_ACTION_TYPE_MACRO    = 0x04
};

/**
 * The size of a macro event (in bytes).
 */
#define MACRO_EVENT_SIZE (10)

/**
 * @brief Every macro data packet seems to have a sum byte after the button byte that alternates between adding 1 and 2 each packet.
 * Another way of thinking about it is half of the numbers in the sum are 1 and half are 2.
 * 
 * Thus, we get the following formula: x / 2 + (2*x) / 2. Which is simplified to (3*x) / 2. Note that this is int division, so there would be no fractional part.
 * 
 * For example, the sum bytes for 6 packets would be: 0x00, 0x01, 0x03, 0x04, 0x06, 0x07
 */
#define MACRO_PACKET_SUM(x) ((3*(x)) / 2)

/**
 * @brief The event count byte in odd macro data packets have 0x80 added to it.
 * 
 */
#define MACRO_PACKET_EVENT_COUNT(x) (((x) % 2) * 0x80)

enum MODIFIER_KEY {
    L_CTRL	= 0b00000001,
    L_SHIFT	= 0b00000010,
    L_ALT	= 0b00000100,
    L_WIN	= 0b00001000,
    R_CTRL	= 0b00010000,
    R_SHIFT	= 0b00100000,
    R_ALT	= 0b01000000,
    R_WIN	= 0b10000000,
} typedef MODIFIER_KEY;

/**
 * @brief Maps the hid usage ids for modifier keys
 * to its corresponding modifier bit.
 * 
 */
#define MACRO_MODIFIER_MAP() {\
    [0xE0] = L_CTRL,\
    [0xE1] = L_SHIFT,\
    [0xE2] = L_ALT,\
    [0xE3] = L_WIN,\
    [0xE4] = R_CTRL,\
    [0xE5] = R_SHIFT,\
    [0xE6] = R_ALT,\
    [0xE7] = R_WIN,\
}

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
    DISABLED            = 0x0000,

    LEFT_CLICK          = 0x0101,
    RIGHT_CLICK         = 0x0102,
    MIDDLE_CLICK        = 0x0103,
    BACK                = 0x0104,
    FORWARD             = 0x0105,
    
    PLAY_PAUSE          = 0x0300,
    STOP                = 0x0301,
    PREVIOUS            = 0x0302,
    NEXT                = 0x0303,
    MUTE                = 0x0304,
    VOLUME_DOWN         = 0x0305,
    VOLUME_UP           = 0x0306,
    
    TASK_MANAGER        = 0x0501,
    SYSTEM_UTILITY      = 0x0502,
    SHOW_DESKTOP        = 0x0503,
    CYCLE_APPS          = 0x0504,
    CLOSE_APPS          = 0x0505,
    CUT                 = 0x0506,
    COPY                = 0x0507,
    PASTE               = 0x0508,
    
    DPI_TOGGLE          = 0x0708,
} typedef SIMPLE_MOUSE_ACTION;

enum MACRO_BINDING {
    MACRO_BINDING_MIDDLE  = 0x0402,
    MACRO_BINDING_BACK    = 0x0403,
    MACRO_BINDING_FORWARD = 0x0404,
    MACRO_BINDING_DPI     = 0x0405
} typedef MACRO_BINDING;

enum MACRO_EVENT_TYPE {
    MACRO_EVENT_TYPE_KEYBOARD = 0x1a,
    MACRO_EVENT_TYPE_MOUSE    = 0x25
} typedef MACRO_EVENT_TYPE;

enum MACRO_REPEAT_MODE {
    MACRO_REPEAT_MODE_ONCE = 0x00,
    MACRO_REPEAT_MODE_TOGGLE_REPEAT = 0x02,
    MACRO_REPEAT_MODE_HOLD_REPEAT = 0x03,
} typedef MACRO_REPEAT_MODE;

/**
 * @brief A struct for a keyboard event.
 */
struct macro_key_event {
    /** A MACRO_EVENT_TYPE value */
    byte event_type;
    byte modifier_keys;
    byte keys[6];
    uint16_t delay_next_action;
} __attribute__((__packed__)) typedef macro_key_event;

/**
 * @brief A struct for a mouse event.
 */
struct macro_mouse_event {
    /** A MACRO_EVENT_TYPE value */
    byte event_type;
    byte button;
    byte _padding;
    uint16_t delay_next_action;
} __attribute__((__packed__)) typedef macro_mouse_event;

/**
 * @brief A struct for a pair of mouse events.
 */
struct macro_mouse_event_pair {
    macro_mouse_event down;
    macro_mouse_event up;
} __attribute__((__packed__)) typedef macro_mouse_event_pair;

/**
 * @brief A union for a macro event.
 */
union marcro_event {
    byte event_data[10];
    macro_key_event key_event;
    macro_mouse_event_pair mouse_event;
} __attribute__((__packed__)) typedef macro_event;

/**
 * Change a binding for a mouse button.
 * 
 * @param dev The mouse device handle
 * @param button The mouse button to re-assign
 * @param action The action to assign to the button
 * @return the number of bytes written or -1 on error
 */
int assign_button_action(hid_device *dev, MOUSE_BUTTON button, uint16_t action);

/**
 * Assign a macro to a mouse button.
 * 
 * @param dev The mouse device handle
 * @param button The mouse button to re-assign
 * @param repeat_mode The repeat behavior of the macro
 * @param events The macro events
 * @param event_count The number of events
 * @return the number of bytes written or -1 on error
 */
int assign_button_macro(hid_device *dev, MOUSE_BUTTON button,  MACRO_REPEAT_MODE repeat_mode, macro_event *events, int event_count);

#endif
