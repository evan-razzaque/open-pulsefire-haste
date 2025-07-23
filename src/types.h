#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "device/rgb.h"
#include "device/mouse.h"
#include "device/buttons.h"
#include "device/sensor.h"

#include "macro_types.h"

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
	int battery_level; // The battery percentage of the mouse
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
	color_options mouse_led; // The mouse's led color and brightness
	GtkColorChooser *color_chooser; // Used to set the color of the mouse's led
	GtkRange *range_brightness; // Used to set the brightness for the mouse's led
} typedef config_color_data;

/**
 * @brief A struct to store button config for the mouse.
 */
struct config_button_data {
	MOUSE_BUTTON selected_button; // The mouse button that is being rebinded
	const MOUSE_BUTTON buttons[BUTTON_COUNT]; // An array of buttons for setting gobject data for each menu button

	// Bindings for the mouse. Each binding is 2 bytes,
	// where the upper byte is the action type and the lower byte is the action value.
	uint16_t bindings[BUTTON_COUNT];
	const uint16_t default_bindings[BUTTON_COUNT]; // Default binding for the mouse buttons
	const char *selected_button_name; // The name of the selected mouse button

	const byte keyboard_keys[1 << 16]; // An array for mapping Gdk keyvals to hid usage ids 
	const char *key_names[256]; // An array for mapping hid usage ids to key names
	uint16_t current_keyboard_action; // The current keyboard action when re-assigning a button to a keyboard action

	GtkMenuButton *menu_button_bindings[BUTTON_COUNT]; // Menu buttons for each mouse button binding
	GtkStack *stack_button_actions; // The stack containing stack pages for each action type (mouse, keyboard, etc)

	GtkWindow *window_keyboard_action; // The window used re-assign a mouse button to a keyboard key
	GtkLabel *label_selected_button; // Display the name of the mouse button being binded to a keyboard key
	GtkLabel *label_pressed_key; // Displays the key that will be assigned to selected mouse button
	GtkEventController *event_key_controller; // Used to listen to key events when assigning a keyboard action
} typedef config_button_data;

/**
 * @brief A struct for storing a single generic event for a macro.
 * Generic macro events, unlike macro events, can only contain one keyboard action
 * or one mouse action, and can either be a pressed event or a released event.
 */
struct generic_macro_event {
	MACRO_ACTION_TYPE action_type; // A MACRO_ACTION_TYPE value
	MACRO_EVENT_TYPE event_type;  // A MACRO_EVENT_TYPE value
	byte action; // The action value
	uint16_t delay; // The delay of this event
	int delay_next_event; // The delay between this event and the next event
} typedef generic_macro_event;

/**
 * @brief A struct for storing a recorded macro.
 */
struct recorded_macro {
	generic_macro_event *events; // The generic macro events
	REPEAT_MODE repeat_mode; // The repeat behavior of the macro
	
	char* name; // The name of the macro
	int generic_event_count; // The number of events
	int generic_event_array_size; // The size of the events array
} typedef recorded_macro;

/**
 * @brief A struct to store macros and macro settings for the mouse.
 */
struct config_macro_data {
	const byte modifier_map[256]; // Maps hid usage ids to modifier bit flags for a macro key event
	const byte mouse_buttons[10]; // Maps GtkGesture mouse button values to mouse button values for a macro mouse event
	const byte repeat_mode_map[3]; // Repeat mode values for a macro
	const char *mouse_button_names[32];

	recorded_macro *macros; // Stores the recorded macros
	int macro_count; // The number of recorded macros
	int macro_array_size; // The size of the macros array
	bool is_recording_macro; // Indicates if the user is recording a macro or not
	/** Whether the user is adding new macro events after unpausing the macro recording or not */
	bool is_resuming_macro_recording;

	/**
	 * Used to hold the value of the last pressed key when recording a macro. 
	 * Should be set to 0 once the same key is released. This is necessary because 
	 * key press events in Gtk are also emitted on key repeats (for some reason), so we use this
	 * variable to prevent the same key down event occuring multiple times in a row.
	 */
	uint32_t last_pressed_key; 

	uint32_t macro_index; // The index of the macro being recorded/edited
	uint32_t macro_previous_event_count; // The previous event count in the macro being edited
	REPEAT_MODE macro_previous_repeat_mode; // The previous repeat mode of the macro being edited

	int macro_indicies[6]; // Used to store the macro index for each button that is assigned to a macro

	GtkGesture *gesture_macro_mouse_events; // Listens to mouse events for macros
	
	// Used to save a macro while claiming the click event before it's added to the macro
	GtkGesture *gesture_button_save_macro_claim_click;
	// Used toggle the recording of the macro while claiming the click event before it's added to the macro
	GtkGesture *gesture_button_record_macro_claim_click;

	GtkEventController *macro_key_events; // Listens to key events for macros
	AdwWrapBox *wrap_box_macro_events; // Displays each generic macro event creating/editing a macro

	GtkButton *button_record_macro; // The actual button used to toggle the macro recording
	GtkImage *image_recording_macro;

	GtkDropDown *drop_down_repeat_mode; // A dropdown to select the macro's repeat behavior

	GtkButton *button_save_macro; // The actual button used to save the macro
	GtkListBox *box_saved_macros; // Holds the MouseMacroButton widgets
	GtkEditable *editable_macro_name; // Contains the name of the macro that is being created/edited
} typedef config_macro_data;

/**
 * @brief A struct to store sensor config for the mouse.
 */
struct config_sensor_data {
	POLLING_RATE polling_rate_value; // The polling rate value for the mouse
	LIFT_OFF_DISTANCE lift_off_distance; // The lift off distance for the mouse

	dpi_settings dpi_config; // The dpi settings for the mouse

	GtkWidget *button_add_dpi_profile; // The button used to add a dpi profile
	GtkCheckButton* check_button_group_dpi_profile; // The check button group for the dpi profiles' check button
	GtkListBox *list_box_dpi_profiles; // Holds each DpiProfileConfig widget
} typedef config_sensor_data;

/**
 * @brief A struct used to store all the variables and structs needed in the application. 
 * This includes mouse data, application widgets, and mouse config.
 */
struct app_data {
	mouse_data *mouse; // The mouse_data struct
	FILE *settings_file; // File used to store mouse settings
	FILE *macros_file; // File used to store recorded macros

	app_widgets *widgets; // Shared application widgets
	
	mouse_battery_data battery_data; // Mouse battery data
	config_color_data color_data; // Mouse led data and settings
	config_button_data button_data; // Mouse button data and settings 
	config_macro_data macro_data; // Macro data and macro bindings for the mouse
	config_sensor_data sensor_data; // Mouse sensor data and settings 
} typedef app_data;

#endif
