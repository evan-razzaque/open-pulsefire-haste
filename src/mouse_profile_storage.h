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
#include "defs.h"

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#define APP_DIR "open-pulsefire-haste" PATH_SEP
#define SELECTED_PROFILE_NAME_FILE ".selected_profile_name"
#define PROFILE_DIR "profiles" PATH_SEP
#define DEFAULT_PROFILE_NAME "default"
#define PROFILE_EXTENSION ".bin"
#define PROFILE_EXTENSION_LENGTH (sizeof(PROFILE_EXTENSION) - 1)
#define PROFILE_NAME_MAX_LENGTH (64)
#define PROFILE_FILENAME_MAX_LENGTH (PROFILE_NAME_MAX_LENGTH + PROFILE_EXTENSION_LENGTH)

// Only when cwd is $XDG_DATA_HOME/APP_DIR
#define PROFILE_PATH_MAX_LENGTH ((sizeof(PROFILE_DIR) - 1) + PROFILE_FILENAME_MAX_LENGTH)

/**
 * @brief A struct containing a mouse profile with its settings and macros.
 */
struct mouse_profile {
	color_options led; // The mouse's led color and brightness

    // Bindings for the mouse button. Each binding is 2 bytes,
	// where the upper byte is the action type and the lower byte is the action value.
	uint16_t bindings[6];

	dpi_settings dpi_config; // The dpi settings for the mouse
	byte polling_rate_value; // A `POLLING_RATE` value
	byte lift_off_distance; // A `LIFT_OFF_DISTANCE` value

    // An array of macro indices for each button that is assigned to a macro. Non-macro bindings must be -1.
    int macro_indices[6];   
    int macro_count;

    // An array containing the profile's recorded macros. MUST be set to NULL when writing the profile to disk.
    recorded_macro *macros;
};

/**
 * @brief Writes the selected profile to $XDG_DATA_HOME/APP_DIR/SELECTED_PROFILE_NAME_FILE.
 * 
 * @param profile_name A buffer to store the profile name into. Must be at least `PROFILE_NAME_MAX_LENGTH + 1` bytes long.
 * @return int 0 if the selected profile name was read or -1 if there was an error 
 */
int save_selected_profile_name(const char *name);

/**
 * @brief Reads the selected profile name into `profile_name`.
 * 
 * @param profile_name A buffer to store the profile name into. Must be at least `PROFILE_NAME_MAX_LENGTH + 1` bytes long.
 * @return int 0 if the selected profile name was read or -1 if there was an error 
 */
int load_selected_profile_name(char *profile_name);

/**
 * @brief Creates the application data directory and its structure
 * if it doesn't exist. On success, the current working directory of the program
 * will change to the application data directory.
 *
 * @return 0 if the directory already exists, or was successfully created. Returns -1 if an error occurred.
 */
int create_data_directory();

/**
 * @brief A function to check if a profile with `name` exists.
 * 
 * @param name The name of the profile
 * @return true if the profile exists, otherwise false
 */
bool profile_file_exists(const char *name);

/**
 * @brief Frees the memory allocated for a mouse profile
 * when it's removed from `app_data->mouse_profiles`.
 * 
 * @param profile The mouse profile being removed
 */
void destroy_profile(mouse_profile *profile);

/**
 * @brief Load a mouse profile from disk. 
 * If the profile file does not exist, a mouse profile will be created.
 * 
 * @param data Application wide data structure
 * @param name The name of the profile
 * @return A `mouse_profile` object if the profile was loaded or NULL if an error has occured
 */
mouse_profile* load_profile_from_file(const char *name, app_data *data);

/**
 * @brief Save a mouse profile to disk.
 * 
 * @param name The name of the profile
 * @param profile The profile to save
 * @param data Application wide data structure
 * @return 0 if the settings were saved or -1 if there was an error
 */
int save_profile_to_file(const char *name, mouse_profile *profile, app_data *data);

/**
 * @brief A function to switch to a different mouse profile. If the profile doesn't exist, a new one is created.
 * 
 * @param name The name of the profile to switch to
 * @param data Application wide data structure
 * @return 0 if the the mouse profile was successfully switched to or -1 if there was an error 
 */
int switch_profile(const char *name, app_data *data);

/**
 * @brief A function to rename a mouse profile.
 * 
 * @param old_name The old profile name
 * @param new_name The new profile name
 * @param data Application wide data structure
 * @return int 0 if the profile was renamed or -1 if there was an error
 */
int rename_profile(const char *old_name, const char *new_name, app_data *data);

/**
 * @brief A function to delete a mouse profile.
 * 
 * @param name The name of the profile to remove
 * @param data Application wide data structure
 * @return int 0 if the profile was deleted or -1 if an error has occured
 */
int delete_profile(const char *name, app_data *data);

#endif