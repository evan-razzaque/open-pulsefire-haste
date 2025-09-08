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
	GtkColorChooser *color_chooser; // Used to set the color of the mouse's led
	GtkRange *range_brightness; // Used to set the brightness for the mouse's led
};

/**
 * Init for mouse led settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_led_init(GtkBuilder *builder, app_data *data);

#endif