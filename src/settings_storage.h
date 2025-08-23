#ifndef SETTINGS_STORAGE_H
#define SETTINGS_STORAGE_H

#include "types.h"

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
 * @brief Load the mouse settings from disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the settings were loaded or -1 if there was an error
 */
int load_settings_from_file(app_data *data);

/**
 * @brief Save the mouse settings to disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the settings were saved or -1 if there was an error
 */
int save_settings_to_file(app_data *data);

#endif