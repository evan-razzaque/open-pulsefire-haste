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

#ifndef MOUSE_PROFILE_STORAGE_H
#define MOUSE_PROFILE_STORAGE_H

#include "types.h"
#include "device/sensor.h"
#include "config_macro.h"

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#define APP_DIR PATH_SEP "open-pulsefire-haste" PATH_SEP
#define DEFAULT_PROFILE_NAME "default.bin"

/**
 * @brief A struct containing a mouse profile with its settings and macros.
 */
struct mouse_profile {
	color_options led;

	uint16_t bindings[6];

	dpi_settings dpi_config;
	byte polling_rate_value;
	byte lift_off_distance;

    struct {
        int macro_count;
        int macro_indicies[6]; // Contains a macro index for each mouse button. Non-macro bindings are -1.
    } macro_info;

    recorded_macro *macros; // An array containing the profile's recorded macros. MUST be set to NULL when writing the profile to disk.
};

/**
 * @brief Creates the application data directory
 * if it doesn't exist. On success, the directory path
 * is stored in `data->app_data_dir`, along with its length
 * in `data->app_data_dir_length`.
 * 
 * @param data Application wide data structure
 * @return 0 if the directory already exists, or was successfully created. Returns -1 if an error occurred.
 */
int create_data_directory(app_data *data);

/**
 * @brief Frees the memory allocated for mouse profile
 * when it's removed from `app_data->mouse_profiles`.
 * 
 * @param profile The mouse profile being removed
 */
void destroy_profile(mouse_profile *profile);

/**
 * @brief A function to delete a mouse profile.
 * 
 * @param name The name of the profile to remove
 * @param data Application wide data structure
 * @return int 0 if the profile was deleted, -1 if an error has occured
 */
int delete_profile(char *name, app_data *data);

/**
 * @brief Load the mouse settings from disk.
 * 
 * @param data Application wide data structure
 * @param name The name of the profile
 * @return A `mouse_profile` object if the profile was found or NULL if an error has occured
 */
mouse_profile* load_profile_from_file(char *name, app_data *data);

/**
 * @brief Save the mouse settings to disk.
 * 
 * @param name The name of the profile
 * @param profile The profile to save
 * @param data Application wide data structure
 * @return 0 if the settings were saved or -1 if there was an error
 */
int save_profile_to_file(char *name, mouse_profile *profile, app_data *data);

/**
 * @brief A function to switch to a different mouse profile.
 * 
 * @param data Application wide data structure
 * @param name The name of the profile to switch to
 * @return int 
 */
int switch_profile(char *name, app_data *data);

#endif