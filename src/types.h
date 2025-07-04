#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>

#include "device/rgb.h"
#include "device/mouse.h"
#include "device/buttons.h"
#include "device/sensor.h"

enum MOUSE_STATE {
	UPDATE,
	SAVE,
	CLOSED
} typedef MOUSE_STATE;

/**
 * @brief An enum for macro event pressed/released states.
 * 
 */
typedef enum MACRO_ACTION_TYPE {
	MACRO_ACTION_TYPE_UP   = 0x00,
	MACRO_ACTION_TYPE_DOWN = 0x01
} MACRO_ACTION_TYPE;

/**
 * @brief All the data required to interact with the mouse.
 */
struct mouse_data {
	GMutex *mutex;
	int *mouse_pipe;
	hid_device *dev;
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
	GtkEventController *event_key_controller;
	

	GtkBox *box_macro;
	GtkEventController *macro_mouse_events, *macro_key_events;
	GtkLabel *label_macro_events;

	GtkAlertDialog *alert;
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
	color_options mouse_led;
	GtkColorChooser *color_chooser;
	GtkRange *range_brightness;
} typedef config_color_data;

/**
 * @brief A struct to store button config for the mouse.
 * 
 */
struct config_button_data {
	MOUSE_BUTTON selected_button;
	MOUSE_BUTTON buttons[BUTTON_COUNT];
	uint16_t bindings[BUTTON_COUNT];
	char selected_button_name[16];

	const byte keyboard_keys[1 << 16];
	const char *key_names[256];
	uint16_t current_keyboard_action;

	GtkMenuButton *menu_button_bindings[BUTTON_COUNT]; // Menu buttons for each mouse button binding
} typedef config_button_data;

/**
 * @brief A struct for storing a single button event for a macro.
 * 
 */
struct generic_macro_event {
	MACRO_EVENT_TYPE event_type;
	MACRO_ACTION_TYPE action_type;
	byte action;
	int delay_next_action;
} typedef generic_macro_event;

struct mouse_macro {
	generic_macro_event *events;
	int generic_event_count;
	int generic_event_array_size;
	char* name;
} typedef mouse_macro;

struct config_macro_data {
	byte modifier_map[256];
	byte mouse_buttons[10];
	const char *mouse_button_names[32];

	mouse_macro *macros;
	int macro_count;
	int macro_array_size;
	bool recording_macro;

	int macro_indicies[6];

	GtkGesture *gesture_macro_mouse_events;
	GtkGesture *gesture_button_confirm_macro;
	GtkButton *button_confirm_macro;
	GMenu *menu_macros;
	GtkEditable *editable_macro_name;
} typedef config_macro_data;

struct config_sensor_data {
	byte polling_rate_value;
	byte lift_off_distance;
	
	dpi_settings dpi_config;
	GtkCheckButton *check_buttons_dpi_profile[5];

	GtkWidget *button_add_dpi_profile;
	GtkCheckButton* check_button_group_dpi_profile;
	GtkListBox *list_box_dpi_profiles;
} typedef config_sensor_data;

/**
 * @brief A struct used to store all the variables and structs needed in the application. 
 * This includes mouse data, application widgets, and mouse config.
 */
struct app_data {
	mouse_data *mouse;
	FILE *settings_file, *macros_file;

	app_widgets *widgets;
	mouse_battery_data battery_data;
	config_color_data color_data;
	config_button_data button_data;
	config_macro_data macro_data;
	config_sensor_data sensor_data;
} typedef app_data;

#endif