#ifndef SETTINGS_STORAGE_H
#define SETTINGS_STORAGE_H

#include "types.h"

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

/**
 * @brief Save the macros to disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the macros were saved or -1 if there was an error
 */
int save_macros_to_file(app_data *data);

/**
 * @brief Load the macros frp, disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the macros were loaded or -1 if there was an error
 */
int load_macros_from_file(app_data *data);

#endif