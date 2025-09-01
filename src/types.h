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

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>
#include "device/mouse.h"

/**
 * @brief Used to cast function pointers to GAction callbacks.
 * 
 */
typedef void (*g_action)(GSimpleAction*, GVariant*, gpointer);

typedef struct config_color_data config_color_data;
typedef struct config_button_data config_button_data;
typedef struct config_macro_data config_macro_data;
typedef struct config_sensor_data config_sensor_data;
typedef struct mouse_profile mouse_profile;

// An enum for describing the current state of the mouse.
typedef enum MOUSE_STATE {
	UPDATE,
	DISCONNECTED,
	IDLE,
	RECONNECT,
	CLOSED
} MOUSE_STATE;

/**
 * @brief Data required to directly interact with the mouse.
 */
struct mouse_data {
	GThread *update_thread; // The thread responsible for updating mouse status and led settings
	GMutex *mutex; // The mutex to lock when sending/recieving data to/from the mouse
	hid_device *dev; // The underlying device handle for the mouse
	
	CONNECTION_TYPE connection_type; // The CONNECTION_TYPE flags
	MOUSE_STATE state; // The MOUSE_STATE value
	int battery_level; // The battery percentage of the mouse. A value of -1 indicates that the battery level is unknown.
	int current_battery_level; // Used to check if the battery level has changed
} typedef mouse_data;

/**
 * @brief The widgets being shared across the entire application.
 */
struct app_widgets {
	GtkBuilder *builder; // The builder used to get widgets
	GtkApplication *app; // The application instance
	GtkWindow *window; // The main application window
	GtkLabel *label_battery; // Displays the mouse's battery level

	GtkStack *stack_main; // The stack containing the main page and the macro page
	GtkBox *box_main; // The box that contains the content of the main page
	GtkOverlay *overlay_main; // Holds the overlay widget to display when the connection to the mouse is lost
	GtkWidget *box_connection_lost; // The widget that displays when the connection to the mouse is lost
} typedef app_widgets;

/**
 * @brief A struct used to store all the variables and structs needed in the application. 
 * This includes mouse data, application widgets, and mouse config.
 */
struct app_data {
	mouse_data *mouse; // The mouse_data struct
	app_widgets *widgets; // Shared application widgets
	
	char *app_data_dir; // XDG data base directory
	int app_data_dir_length; // Length of `app_data_dir`, including the null-byte

	FILE *profile_file; // File used to store mouse settings
	char *profile_name; // Name of the profile 
	
	GHashTable *mouse_profiles;
	mouse_profile *profile;

	config_color_data *color_data; // Mouse led data and settings
	config_button_data *button_data; // Mouse button data and settings 
	config_macro_data *macro_data; // Macro data and macro bindings for the mouse
	config_sensor_data *sensor_data; // Mouse sensor data and settings 
} typedef app_data;

#endif
