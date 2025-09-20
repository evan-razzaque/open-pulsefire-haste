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

#ifndef CONFIG_SENSOR_H
#define CONFIG_SENSOR_H

#include <gtk/gtk.h>
#include "device/sensor.h"
#include "types.h"

#define CHANGE_POLLING_RATE "change-polling-rate"
#define CHANGE_LIFT_OFF_DISTANCE "change-lift-off-distance"
#define SELECT_DPI_PROFILE "select-dpi-profile"

/**
 * @brief A struct to store sensor config for the mouse.
 */
struct config_sensor_data {
	bool user_changed_dpi_profile; // A flag that indicates if a dpi profile change was done by the user (GUI) or the mouse (DPI toggle event)

	GtkWidget *button_add_dpi_profile; // The button used to add a dpi profile
	GtkCheckButton *check_button_group_dpi_profile; // The check button group for the dpi profiles' check button
	GtkListBox *list_box_dpi_profiles; // Holds each DpiProfileConfig widget
};

/**
 * @brief A struct for used as argurments
 * for `update_dpi_profile_selection`.
 */
struct dpi_profile_selection_args {
	byte index; // The index of the dpi profile row to select
	app_data *data; // Application wide data structure
	// A function pointer to free this struct instance. Must be set to NULL if the struct is not heap-allocated.
	GDestroyNotify free_func;
} typedef dpi_profile_selection_args;

/**
 * @brief A function to change which dpi profile row is visually selected
 * when a DPI toggle event is emitted from the mouse.
 *
 * @param args A dpi_profile_selection_args struct instance
 */
void update_dpi_profile_selection(dpi_profile_selection_args *args);

/**
 * @brief A function to create dpi profile rows for each dpi profile.
 *
 * @param dpi_config The dpi config for the mouse
 * @param data Application wide data struture
 */
void create_dpi_profile_rows(dpi_settings *dpi_config, app_data *data);

/**
 * Init for mouse sensor related settings.
 *
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_sensor_init(GtkBuilder* builder, app_data* data);

#endif