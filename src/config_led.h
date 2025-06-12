#ifndef CONFIG_LED_H
#define CONFIG_LED_H

#include <gtk/gtk.h>

#include "types.h"

/**
 * Init for changing mouse led settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_led_init(GtkBuilder *builder, app_data *data);

int update_color(void *data);

void update_brightness(GtkRange *range_brightness, void *mouse_led_brightness);

#endif

