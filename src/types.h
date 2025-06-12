#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "device/rgb.h"
#include "device/mouse.h"

enum MOUSE_STATE {
	UPDATE,
	CLOSED
} typedef MOUSE_STATE;

struct mouse_data {
	hid_device *dev;
	color_options *led;
	CONNECTION_TYPE type;
	MOUSE_STATE state;
	int battery_level;
} typedef mouse_data;

struct app_widgets {
	GtkWindow *window;
	GtkLabel *label_battery;
	GtkLabel *label_pressed_key;
	GtkColorChooser *color_chooser;
    GtkRange *range_brightness;
	GtkEventController *event_key_controller;
} typedef app_widgets;

struct config_color_data {
	color_options *mouse_led;
	GtkColorChooser *color_chooser;
} typedef config_color_data;

struct mouse_battery_data {
	mouse_data *mouse;
	GtkLabel *label_battery;
} typedef mouse_battery_data;

struct mouse_action_values {
	uint16_t mouse[6];
	uint16_t media[7];
} typedef mouse_action_values;

struct app_data {
	mouse_data *mouse;
	app_widgets *widgets;
	mouse_action_values *action_values;
	config_color_data color_data;
} typedef app_data;

#endif