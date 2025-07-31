#ifndef CONFIG_LED_H
#define CONFIG_LED_H

#include <gtk/gtk.h>
#include "device/rgb.h"
#include "types.h"

/**
 * @brief A struct to store color config for the mouse.
 * 
 */
struct config_color_data {
	color_options mouse_led; // The mouse's led color and brightness
	GtkColorChooser *color_chooser; // Used to set the color of the mouse's led
	GtkRange *range_brightness; // Used to set the brightness for the mouse's led
} typedef config_color_data;

/**
 * Init for mouse led settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_led_init(GtkBuilder *builder, app_data *data);

#endif