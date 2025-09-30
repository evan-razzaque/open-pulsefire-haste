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

#ifndef CONFIG_BUTTONS_H
#define CONFIG_BUTTONS_H

#include <gtk/gtk.h>
#include "device/buttons.h"
#include "types.h"

#define BUTTON_ASSIGN_ERROR_INVALID_ASSIGNMENT (-2)

#define MOUSE_ACTION_NAMES {\
    [LEFT_CLICK     & 0x00ff] = "Left Click",\
    [RIGHT_CLICK    & 0x00ff] = "Right Click",\
    [MIDDLE_CLICK   & 0x00ff] = "Middle Click",\
    [BACK           & 0x00ff] = "Back",\
    [FORWARD        & 0x00ff] = "Forward"\
}

#define MEDIA_ACTION_NAMES {\
    [PLAY_PAUSE     & 0x00ff] = "Play/Pause",\
    [STOP           & 0x00ff] = "Stop",\
    [PREVIOUS       & 0x00ff] = "Previous",\
    [NEXT           & 0x00ff] = "Next",\
    [MUTE           & 0x00ff] = "Volume Mute",\
    [VOLUME_DOWN    & 0x00ff] = "Volume Down",\
    [VOLUME_UP      & 0x00ff] = "Volume Up",\
}

#define SHORTCUT_ACTION_NAMES {\
    [TASK_MANAGER   & 0x00ff] = "Launch Task Mananger",\
    [SYSTEM_UTILITY & 0x00ff] = "Open System Utility",\
    [SHOW_DESKTOP   & 0x00ff] = "Show desktop",\
    [CYCLE_APPS     & 0x00ff] = "Cycle apps",\
    [CLOSE_APPS     & 0x00ff] = "Close window",\
    [CUT            & 0x00ff] = "Cut",\
    [COPY           & 0x00ff] = "Copy",\
    [PASTE          & 0x00ff] = "Paste"\
}

#define SIMPLE_ACTION_NAMES {\
    [MOUSE_ACTION_TYPE_DISABLED] = (const char *[]) {\
        [DISABLED       & 0x00ff] = "Disabled"\
    },\
    [MOUSE_ACTION_TYPE_MOUSE] = (const char *[])  {\
        [LEFT_CLICK     & 0x00ff] = "Left Click",\
        [RIGHT_CLICK    & 0x00ff] = "Right Click",\
        [MIDDLE_CLICK   & 0x00ff] = "Middle Click",\
        [BACK           & 0x00ff] = "Back",\
        [FORWARD        & 0x00ff] = "Forward"\
    },\
    [MOUSE_ACTION_TYPE_MEDIA] = (const char *[])  {\
        [PLAY_PAUSE     & 0x00ff] = "Play/Pause",\
        [STOP           & 0x00ff] = "Stop",\
        [PREVIOUS       & 0x00ff] = "Previous",\
        [NEXT           & 0x00ff] = "Next",\
        [MUTE           & 0x00ff] = "Volume Mute",\
        [VOLUME_DOWN    & 0x00ff] = "Volume Down",\
        [VOLUME_UP      & 0x00ff] = "Volume Up",\
    },\
    [MOUSE_ACTION_TYPE_SHORTCUT] = (const char *[])  {\
        [TASK_MANAGER   & 0x00ff] = "Launch Task Mananger",\
        [SYSTEM_UTILITY & 0x00ff] = "Open System Utility",\
        [SHOW_DESKTOP   & 0x00ff] = "Show desktop",\
        [CYCLE_APPS     & 0x00ff] = "Cycle apps",\
        [CLOSE_APPS     & 0x00ff] = "Close window",\
        [CUT            & 0x00ff] = "Cut",\
        [COPY           & 0x00ff] = "Copy",\
        [PASTE          & 0x00ff] = "Paste"\
    },\
    [MOUSE_ACTION_TYPE_DPI] = (const char *[])  {\
        [DPI_TOGGLE     & 0x00ff] = "DPI Toggle"\
    }\
}

/**
 * @brief A struct to store button config for the mouse.
 */
struct config_button_data {
	MOUSE_BUTTON selected_button; // The mouse button that is being rebinded
	const MOUSE_BUTTON buttons[BUTTON_COUNT]; // An array of buttons for setting gobject data for each menu button

	const uint16_t default_bindings[BUTTON_COUNT]; // Default binding for the mouse buttons
	const char *selected_button_name; // The name of the selected mouse button

	const byte keyboard_keys[1 << 16]; // An array for mapping Gdk keyvals to hid usage ids
	uint16_t current_keyboard_action; // The current keyboard action when re-assigning a button to a keyboard action

    const char **simple_action_names[MOUSE_ACTION_TYPE_DPI + 1]; // A 2d array for storing the names of simple mouse actions, grouped by action type
	const char *key_names[256]; // An array for mapping hid usage ids to key names

	GtkMenuButton *menu_button_bindings[BUTTON_COUNT]; // Menu buttons for each mouse button binding
	GtkStack *stack_button_actions; // The stack containing stack pages for each action type (mouse, keyboard, etc)

	GtkWindow *window_keyboard_action; // The window used re-assign a mouse button to a keyboard key
	GtkLabel *label_selected_button; // Display the name of the mouse button being binded to a keyboard key
	GtkLabel *label_pressed_key; // Displays the key that will be assigned to selected mouse button
	GtkEventController *event_key_controller; // Used to listen to key events when assigning to a keyboard action
};

/**
 * @brief Gets the menu button that has its popover shown.
 *
 * @param button_data The button data for the mouse
 * @return GtkMenuButton* the active menu button widget
 */
GtkMenuButton* get_active_menu_button(config_button_data *button_data);

/**
 * @brief Sets the visibility of a menu button popover
 *
 * @param self The menu button that contains the popover
 * @param visible Whether the popover should be visible or not
 */
void menu_button_set_popover_visibility(GtkMenuButton *self, bool visible);

/**
 * @brief A method to set the stack's page.
 *
 * @param stack The GtkStack widget
 * @param page The page number
 */
void gtk_stack_set_page(GtkStack *stack, uint32_t page);

/**
 * @brief Re-binds a mouse button.
 *
 * @param button The button being rebound
 * @param action The action to bind to
 * @param menu_button_active The menu button corresponding to the button being re-binded
 * @param app_data Application wide data structure
 * @return 0 if the button was assigned successfully or a `BUTTON_ASSIGN_ERROR` value on error.
 */
int assign_button(MOUSE_BUTTON button, uint16_t action, app_data *data);

/**
 * Init for mouse button remapping.
 *
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_buttons_init(GtkBuilder *builder, app_data *data);

#endif