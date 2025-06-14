#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "device/rgb.h"
#include "device/mouse.h"
#include "device/buttons.h"

/**
 * @brief Convenience macro for populating mouse_action_values::actions.
 * 
 * @param action_value mouse_action_values instance
 */
#define fill_actions_array(action_values)\
	(action_values)->actions[0] = (action_values)->disabled;\
	(action_values)->actions[1] = (action_values)->mouse;\
	(action_values)->actions[2] = (action_values)->media;\
	(action_values)->actions[3] = (action_values)->windows_shortcut

enum MOUSE_STATE {
	UPDATE,
	CLOSED
} typedef MOUSE_STATE;

struct mouse_data {
	GMutex *mutex;
	hid_device *dev;
	color_options *led;
	CONNECTION_TYPE type;
	MOUSE_STATE state;
	int battery_level;
} typedef mouse_data;

struct app_widgets {
	GtkApplication *app;
	GtkWindow *window;
	GtkLabel *label_battery;
	GtkLabel *label_pressed_key;
	GtkColorChooser *color_chooser;
    GtkRange *range_brightness;
	GtkEventController *event_key_controller;
	GtkMenuButton *menu_button_bindings[6]; // Menu buttons for each mouse button binding
} typedef app_widgets;

struct mouse_battery_data {
	mouse_data *mouse;
	GtkLabel *label_battery;
} typedef mouse_battery_data;

struct config_color_data {
	color_options *mouse_led;
	GtkColorChooser *color_chooser;
} typedef config_color_data;

/**
 * Used for storing action values for each action type.
 */
struct mouse_action_values {
	uint16_t disabled[1];
	uint16_t mouse[6];
	uint16_t media[7];
	uint16_t windows_shortcut[8];
	uint16_t *actions[4]; // Holds pointers for each action value array, must be set manually
} typedef mouse_action_values;

struct config_button_data {
	hid_device *dev;
	mouse_action_values *action_values;
	MOUSE_BUTTON button;
	MOUSE_BUTTON buttons[6];
} typedef config_button_data;

struct app_data {
	mouse_data *mouse;
	app_widgets *widgets;
	mouse_battery_data battery_data;
	config_color_data color_data;
	config_button_data button_data;
} typedef app_data;

#endif