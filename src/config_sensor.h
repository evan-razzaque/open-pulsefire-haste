#ifndef CONFIG_SENSOR_H
#define CONFIG_SENSOR_H

#include <gtk/gtk.h>
#include "device/sensor.h"
#include "types.h"

/**
 * @brief A struct to store sensor config for the mouse.
 */
struct config_sensor_data {
	POLLING_RATE polling_rate_value; // The polling rate value for the mouse
	LIFT_OFF_DISTANCE lift_off_distance; // The lift off distance for the mouse

	dpi_settings dpi_config; // The dpi settings for the mouse

	GtkWidget *button_add_dpi_profile; // The button used to add a dpi profile
	GtkCheckButton* check_button_group_dpi_profile; // The check button group for the dpi profiles' check button
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
 * @param args A dpi_profile_selection_args struct instance.
 */
void update_dpi_profile_selection(dpi_profile_selection_args *args);

/**
 * Init for mouse sensor related settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_sensor_init(GtkBuilder* builder, app_data* data);

#endif