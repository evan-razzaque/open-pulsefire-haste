#ifndef APPLICATION_H
#define APPLICATION_H

#include <gtk/gtk.h>

#include "types.h"

/**
 * @brief Sets the settings for the mouse.
 * 
 * @param data Application wide data structure
 */
void load_mouse_settings(app_data *data);

/**
 * @brief Saves the settings to the mouse.
 * 
 * @param self The save button
 * @param mouse mouse_data instance
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