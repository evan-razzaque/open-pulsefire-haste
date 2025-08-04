#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <adwaita.h>
#include "device/mouse.h"

/**
 * @brief Used to cast function pointers to GAction callbacks.
 * 
 */
typedef void (*g_action)(GSimpleAction*, GVariant*, gpointer);

typedef struct config_color_data config_color_data;
typedef struct config_button_data config_button_data;
typedef struct config_macro_data config_macro_data;
typedef struct config_sensor_data config_sensor_data;

// An enum for describing the current state of the mouse.
typedef enum MOUSE_STATE {
	UPDATE,
	CONNECTED,
	DISCONNECTED,
	NOT_FOUND,
	RECONNECT,
	CLOSED
} MOUSE_STATE;

/**
 * @brief Data required to directly interact with the mouse.
 */
struct mouse_data {
	GMutex *mutex; // The mutex to lock when sending/recieving data to/from the mouse
	hid_device *dev; // The underlying device for the mouse
	
	CONNECTION_TYPE type; // The CONNECTION_TYPE flags
	MOUSE_STATE state; // The MOUSE_STATE value
	int battery_level; // The battery percentage of the mouse. A value of -1 indicates that the battery level is unknown.
	int current_battery_level; // Used to check if the battery level has changed
} typedef mouse_data;

/**
 * @brief The widgets being shared across the entire application.
 */
struct app_widgets {
	GtkBuilder *builder; // The builder used to get widgets from window.ui
	GtkApplication *app; // The application instance
	GtkWindow *window; // The main application window
	GtkLabel *label_battery; // Displays the mouse's battery level

	GtkStack *stack_main; // The stack containing the main page and the macro page
	GtkBox *box_main; // The box that contains the content of the main page
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
 * @brief A struct used to store all the variables and structs needed in the application. 
 * This includes mouse data, application widgets, and mouse config.
 */
struct app_data {
	mouse_data *mouse; // The mouse_data struct
	app_widgets *widgets; // Shared application widgets
	
	FILE *settings_file; // File used to store mouse settings
	FILE *macros_file; // File used to store recorded macros
	
	mouse_battery_data battery_data; // Mouse battery data
	config_color_data *color_data; // Mouse led data and settings
	config_button_data *button_data; // Mouse button data and settings 
	config_macro_data *macro_data; // Macro data and macro bindings for the mouse
	config_sensor_data *sensor_data; // Mouse sensor data and settings 
} typedef app_data;

#endif
