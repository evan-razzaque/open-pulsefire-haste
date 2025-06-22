#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "device/rgb.h"
#include "device/mouse.h"
#include "device/buttons.h"

enum MOUSE_STATE {
	UPDATE,
	CLOSED
} typedef MOUSE_STATE;

/**
 * @brief All the data required to interact with the mouse.
 * 
 */
struct mouse_data {
	GMutex *mutex;
	hid_device *dev;
	color_options *led;
	CONNECTION_TYPE type;
	MOUSE_STATE state;
	int battery_level;
} typedef mouse_data;

/**
 * @brief The application widgets being shared across the entire application.
 * 
 */
struct app_widgets {
	GtkApplication *app;
	GtkWindow *window;
	GtkWindow *window_keyboard_action;
	GtkOverlay *overlay;
	GtkLabel *label_battery;
	GtkLabel *label_selected_button, *label_pressed_key;
	GtkColorChooser *color_chooser;
    GtkRange *range_brightness;
	GtkEventController *event_key_controller;
	GtkMenuButton *menu_button_bindings[6]; // Menu buttons for each mouse button binding
	GtkPopover *active_popover;
	GtkBox *box_macro;
} typedef app_widgets;

/**
 * @brief A struct to store battery data for the mosue.
 * 
 */
struct mouse_battery_data {
	mouse_data *mouse;
	GtkLabel *label_battery;
} typedef mouse_battery_data;

/**
 * @brief A struct to store color config for the mouse.
 * 
 */
struct config_color_data {
	color_options *mouse_led;
	GtkColorChooser *color_chooser;
} typedef config_color_data;

/**
 * @brief A struct to store button config for the mouse.
 * 
 */
struct config_button_data {
	hid_device *dev;
	MOUSE_BUTTON selected_button;
	MOUSE_BUTTON buttons[6];
	uint16_t bindings[6];
	char selected_button_name[16];

	const byte keyboard_keys[1 << 16];
	const char *key_names[256];
	uint16_t current_keyboard_action;
} typedef config_button_data;

/**
 * @brief A struct used to store all the variables and structs needed in the application. 
 * This includes mouse data, application widgets, and mouse config.
 */
struct app_data {
	mouse_data *mouse;
	app_widgets *widgets;
	mouse_battery_data battery_data;
	config_color_data color_data;
	config_button_data button_data;
} typedef app_data;

#endif