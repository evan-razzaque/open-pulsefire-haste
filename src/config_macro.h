/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#ifndef CONFIG_MACRO_H
#define CONFIG_MACRO_H

#include <gtk/gtk.h>
#include "device/buttons.h"
#include "types.h"

/**
 * Convieniece macro for a key(s) down event
 *
 * @param modifier The modifier keys
 * @param delay How long to wait before executing the next event (in milliseconds)
 * @param key_ids HID key usage ids (max 6)
 * @return A macro event object
 */
#define KEYBOARD_EVENT_DOWN(modifier, delay, key_ids...)\
    (macro_event) {.key_event = {.action_type = MACRO_ACTION_TYPE_KEYBOARD, .modifier_keys = modifier, .keys = {key_ids}, .delay_next_action = delay}}

/**
 * Convieniece macro for a key(s) up event
 *
 * @param delay long to wait before executing the next event (in milliseconds)
 * @return A macro event object
 */
#define KEYBOARD_EVENT_UP(delay)\
    (macro_event) {.key_event = {.action_type = MACRO_ACTION_TYPE_KEYBOARD, .delay_next_action = delay}}

/**
 * Convieniece macro for a mouse event.
 *   
 * @param btn The mouse button
 * @param duration The duration of mouse down event
 * @param delay How long to wait before executing the next event (in milliseconds)
 */
#define MOUSE_EVENT(btn, duration, delay)\
    (macro_event) {.mouse_event = {\
        .down = {.action_type = MACRO_ACTION_TYPE_MOUSE, .button = btn, .delay_next_action = duration},\
        .up = {.action_type = MACRO_ACTION_TYPE_MOUSE, .delay_next_action = delay}\
    }}

/**
 * @brief Used to map GTK mouse button values to mouse button values for macros.
 */
#define MOUSE_MAP {\
    [1] = GENERIC_EVENT_BUTTON_LEFT   ,\
    [3] = GENERIC_EVENT_BUTTON_RIGHT  ,\
    [2] = GENERIC_EVENT_BUTTON_MIDDLE ,\
    [8] = GENERIC_EVENT_BUTTON_BACK   ,\
    [9] = GENERIC_EVENT_BUTTON_FORWARD\
}

#define MOUSE_BUTTON_NAMES {\
    [GENERIC_EVENT_BUTTON_LEFT]    = "Left Click",\
    [GENERIC_EVENT_BUTTON_RIGHT]   = "Right Click",\
    [GENERIC_EVENT_BUTTON_MIDDLE]  = "Middle Click",\
    [GENERIC_EVENT_BUTTON_BACK]    = "Mouse 4",\
    [GENERIC_EVENT_BUTTON_FORWARD] = "Mouse 5"\
}

/**
 * @brief An enum for generic macro event button pressed/released states.
 * 
 */
typedef enum MACRO_EVENT_TYPE {
	MACRO_EVENT_TYPE_UP   = 0x00,
	MACRO_EVENT_TYPE_DOWN = 0x01
} MACRO_EVENT_TYPE;

enum GENERIC_ACTION_EVENT_TYPES {
    KEY_DOWN   = (MACRO_ACTION_TYPE_KEYBOARD << 8) + MACRO_EVENT_TYPE_DOWN,
    KEY_UP     = (MACRO_ACTION_TYPE_KEYBOARD << 8) + MACRO_EVENT_TYPE_UP,
    MOUSE_DOWN = (MACRO_ACTION_TYPE_MOUSE    << 8) + MACRO_EVENT_TYPE_DOWN,
    MOUSE_UP   = (MACRO_ACTION_TYPE_MOUSE    << 8) + MACRO_EVENT_TYPE_UP
} typedef GENERIC_ACTION_EVENT_TYPES;

/**
 * @brief A struct for storing a single generic event for a macro.
 * Generic macro events, unlike macro events, can only contain one keyboard action
 * or one mouse action, and can either be a pressed event or a released event.
 */
struct generic_macro_event {
	MACRO_ACTION_TYPE action_type; // A MACRO_ACTION_TYPE value
	MACRO_EVENT_TYPE event_type;  // A MACRO_EVENT_TYPE value
	byte action; // The action value
	uint16_t delay; // The delay between this event and the previous event
	time_t delay_next_event; // The delay between this event and the next event
} typedef generic_macro_event;

/**
 * @brief A struct for storing a recorded macro.
 */
struct recorded_macro {
	generic_macro_event *events; // The generic macro events
	REPEAT_MODE repeat_mode; // The repeat behavior of the macro
	
	char* name; // The name of the macro
	int generic_event_count; // The number of events
	int generic_event_array_size; // The size of the events array
} typedef recorded_macro;

/**
 * @brief A struct to store macros and macro settings for the mouse.
 */
struct config_macro_data {
	const byte modifier_map[256]; // Maps hid usage ids to modifier bit flags for a macro key event
	const byte mouse_buttons[10]; // Maps GtkGesture mouse button values to mouse button values for a macro mouse event
	const byte repeat_mode_map[3]; // Repeat mode values for a macro
	const char *mouse_button_names[32];

	recorded_macro *macros; // Stores the recorded macros
	int macro_count; // The number of recorded macros
	int macro_array_size; // The size of the macros array
	bool is_recording_macro; // Indicates if the user is recording a macro or not
	/** Whether the user is adding new macro events after unpausing the macro recording or not */
	bool is_resuming_macro_recording;

	/**
	 * Used to hold the value of the last pressed key when recording a macro. 
	 * Should be set to 0 once the same key is released. This is necessary because 
	 * key press events in Gtk are also emitted on key repeats (for some reason), so we use this
	 * variable to prevent the same key down event occuring multiple times in a row.
	 */
	uint32_t last_pressed_key; 

	uint32_t macro_index; // The index of the macro being recorded/edited
	uint32_t macro_previous_event_count; // The previous event count in the macro being edited
	REPEAT_MODE macro_previous_repeat_mode; // The previous repeat mode of the macro being edited

	int macro_indicies[6]; // Used to store the macro index for each button that is assigned to a macro

	GtkGesture *gesture_macro_mouse_events; // Listens to mouse events for macros
	
	// Used to save a macro while claiming the click event before it's added to the macro
	GtkGesture *gesture_button_save_macro_claim_click;
	// Used toggle the recording of the macro while claiming the click event before it's added to the macro
	GtkGesture *gesture_button_record_macro_claim_click;

	GtkEventController *macro_key_events; // Listens to key events for macros
	GtkBox *box_wrap_box_macro_events; // Holds the WrapBox for the generic macro events
	AdwWrapBox *wrap_box_macro_events; // Displays each generic macro event when creating/editing a macro

	GtkButton *button_record_macro; // The actual button used to toggle the macro recording
	GtkImage *image_recording_macro;

	GtkDropDown *drop_down_repeat_mode; // A dropdown to select the macro's repeat behavior

	GtkButton *button_save_macro; // The actual button used to save the macro
	GtkListBox *box_saved_macros; // Holds the MouseMacroButton widgets
	GtkEditable *editable_macro_name; // Contains the name of the macro that is being created/edited
};

/**
 * @brief Assigns a macro to a mouse button.
 * 
 * @param macro_index The index of the macro
 * @param button The button number being re-assigned
 * @param data Application wide data structure
 */
void assign_macro(uint32_t macro_index, byte button, app_data *data);

/**
 * Init for macros.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_macro_init(GtkBuilder *builder, app_data *data);

#endif