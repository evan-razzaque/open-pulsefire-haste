#ifndef MOUSE_CONFIG_H
#define MOUSE_CONFIG_H

#include <gtk/gtk.h>
#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"

/**
 * Init for mouse led settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_led_init(GtkBuilder *builder, app_data *data);

/**
 * Init for mouse button remapping.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_buttons_init(GtkBuilder *builder, app_data *data);

#endif
