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

#include <gtk/gtk.h>
#include <gtk/gtkapplication.h>

#include "config_led.h"
#include "types.h"
#include "device/rgb.h"

/**
 * @brief Updates the led rgb color from the color chooser wigdet
 * 
 * @param data Application wide data structure
 * @return return value telling gtk to keep this function in its main loop
 */
static int update_color(config_color_data *color_data) {
	color_options *mouse_led = &color_data->mouse_led;
    
	GdkRGBA color = {};
	gtk_color_chooser_get_rgba(color_data->color_chooser, &color);

	mouse_led->red = (byte) (color.red * 255);
	mouse_led->green = (byte) (color.green * 255);
	mouse_led->blue = (byte) (color.blue * 255);

	return G_SOURCE_CONTINUE;
}

/**
 * @brief A function to update the mouse led brightness
 * 
 * @param range_brightness The range from the slider wigdet to obtain the brightness from
 * @param mouse_led_brightness A pointer to the mouse led brightness
 */
static void update_brightness(GtkRange *range_brightness, byte *mouse_led_brightness) {
    *mouse_led_brightness = gtk_range_get_value(range_brightness);
}

void app_config_led_init(GtkBuilder *builder, app_data *data) {
    data->color_data->range_brightness = GTK_RANGE(GTK_WIDGET(gtk_builder_get_object(builder, "scaleBrightness")));
	data->color_data->color_chooser = GTK_COLOR_CHOOSER(GTK_WIDGET(gtk_builder_get_object(builder, "colorChooserLed")));;

	color_options led = data->color_data->mouse_led;

	const GdkRGBA rgba = {
		led.red / 255.0,
		led.green / 255.0,
		led.blue / 255.0, 
		1
	};

	gtk_color_chooser_set_rgba(data->color_data->color_chooser, &rgba);
	gtk_range_set_value(data->color_data->range_brightness, data->color_data->mouse_led.brightness);

    g_timeout_add(10, G_SOURCE_FUNC(update_color), data->color_data);
    g_signal_connect(data->color_data->range_brightness, "value-changed", G_CALLBACK(update_brightness), &(data->color_data->mouse_led.brightness));
}
