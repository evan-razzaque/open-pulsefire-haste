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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <gtk/gtk.h>

#include "device/buttons.h"
#include "types.h"

enum {
    STACK_PAGE_MAIN,
    STACK_PAGE_MACRO,
    STACK_PAGE_DEVICE_NOT_FOUND
};

/**
 * @brief Displays an overlay when the connection to the mouse
 * is lost or the mouse goes to sleep (wireless only).
 * 
 * @param data Application wide data structure
 */
void show_connection_lost_overlay(app_data *data);

/**
 * @brief Removes the overlay displayed by `show_connection_lost_overlay`
 * when the mouse connection is found or the mouse wakes up.
 * 
 * @param data Application wide data structure
 */
void remove_connnection_lost_overlay(app_data *data);

/**
 * @brief A function to hides and disable the mouse settings screen.
 * 
 * @param data Application wide data structure
 */
void hide_mouse_settings_visibility(app_data *data);

/**
 * @brief A function to show and enable the mouse settings screen.
 * 
 * @param data Application wide data structure
 */
void show_mouse_settings_visibility(app_data *data);

/**
 * @brief A function switch the displayed page in a GtkStack
 * 
 * @param stack The GtkStack widget
 * @param button The GtkActionable of the button for switching the stack page
 */
void switch_stack_page(GtkStack *stack, GtkActionable* button);

/**
 * @brief A function to update the label of a menu button for a mouse button.
 * 
 * @param button The index of the mouse button
 * @param action The new action of the mouse button
 * @param data Application wide data structure
 */
void update_menu_button_label(MOUSE_BUTTON button, uint16_t action, app_data *data);

/**
 * @brief Loads the active mouse profile to the mouse.
 * 
 * @param data Application wide data structure
 */
void load_mouse_profile_to_mouse(app_data *data);

/**
 * @brief Saves the settings to the mouse.
 * 
 * @param self The save button
 * @param data Application wide data structure
 */
void save_mouse_settings(GtkWidget *self, app_data *data);

/**
 * @brief A function to setup and activate the application.
 * 
 * @param app GtkApplication instance
 * @param data Application wide data structure
 */
void activate(GtkApplication *app, app_data *data);

#endif