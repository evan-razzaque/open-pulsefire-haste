#ifndef MOUSE_CONFIG_H
#define MOUSE_CONFIG_H

#include <gtk/gtk.h>
#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"

enum {
    STACK_PAGE_MAIN,
    STACK_PAGE_MACRO,
    STACK_PAGE_DEVICE_NOT_FOUND
};

/**
 * @brief Used to cast function pointers to GAction callbacks.
 * 
 */
typedef void (*g_action)(GSimpleAction*, GVariant*, gpointer);

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data)\
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

/**
 * Convieniece macro for a key(s) down event
 *
 * @param modifier The modifier keys
 * @param delay How long to wait before executing the next event (in milliseconds)
 * @param key_ids HID key usage ids (max 6)
 * @return A macro event object
 */
#define KEYBOARD_EVENT_DOWN(modifier, delay, key_ids...)\
    (macro_event) {.key_event = {.action_type = MACRO_ACTION_TYPE_KEYBOARD, .modifier_keys = modifier, .keys = {key_ids}, .delay_next_action = delay}}

/**
 * Convieniece macro for a key(s) up event
 *
 * @param delay long to wait before executing the next event (in milliseconds)
 * @return A macro event object
 */
#define KEYBOARD_EVENT_UP(delay)\
    (macro_event) {.key_event = {.action_type = MACRO_ACTION_TYPE_KEYBOARD, .delay_next_action = delay}}

/**
 * Convieniece macro for a mouse event.
 *   
 * @param btn The mouse button
 * @param duration The duration of mouse down event
 * @param delay How long to wait before executing the next event (in milliseconds)
 */
#define MOUSE_EVENT(btn, duration, delay)\
    (macro_event) {.mouse_event = {\
        .down = {.action_type = MACRO_ACTION_TYPE_MOUSE, .button = btn, .delay_next_action = duration},\
        .up = {.action_type = MACRO_ACTION_TYPE_MOUSE, .delay_next_action = delay}\
    }}

/**
 * @brief Used to map GTK mouse button values to mouse button values for macros.
 */
#define MOUSE_MAP() {\
    [1] = 0x01,\
    [3] = 0x02,\
    [2] = 0x04,\
    [8] = 0x08,\
    [9] = 0x10\
}

#define MOUSE_BUTTON_NAMES() {\
    [0x01] = "Left Click",\
    [0x02] = "Right Click",\
    [0x04] = "Middle Click",\
    [0x08] = "Mouse 4",\
    [0x10] = "Mouse 5"\
}

#define SIMPLE_ACTION_NAMES() {\
    [DISABLED     >> 8] = {\
        [DISABLED       & 0x00ff] = "Disabled"\
    },\
    [LEFT_CLICK   >> 8] = {\
        [LEFT_CLICK     & 0x00ff] = "Left Click",\
        [RIGHT_CLICK    & 0x00ff] = "Right Click",\
        [MIDDLE_CLICK   & 0x00ff] = "Middle Click",\
        [BACK           & 0x00ff] = "Back",\
        [FORWARD        & 0x00ff] = "Forward"\
    },\
    [PLAY_PAUSE  >> 8] = {\
        [PLAY_PAUSE     & 0x00ff] = "Play/Pause",\
        [STOP           & 0x00ff] = "Stop",\
        [PREVIOUS       & 0x00ff] = "Previous",\
        [NEXT           & 0x00ff] = "Next",\
        [MUTE           & 0x00ff] = "Volume Mute",\
        [VOLUME_DOWN    & 0x00ff] = "Volume Down",\
        [VOLUME_UP      & 0x00ff] = "Volume Up",\
    },\
    [TASK_MANAGER >> 8] = {\
        [TASK_MANAGER   & 0x00ff] = "Launch Task Mananger",\
        [SYSTEM_UTILITY & 0x00ff] = "Open System Utility",\
        [SHOW_DESKTOP   & 0x00ff] = "Show desktop",\
        [CYCLE_APPS     & 0x00ff] = "Cycle apps",\
        [CLOSE_APPS     & 0x00ff] = "Close window",\
        [CUT            & 0x00ff] = "Cut",\
        [COPY           & 0x00ff] = "Copy",\
        [PASTE          & 0x00ff] = "Paste"\
    },\
    [DPI_TOGGLE   >> 8] = {\
        [DPI_TOGGLE     & 0x00ff] = "DPI Toggle"\
    }\
}

int toggle_mouse_settings_visibility(app_data *data);

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

/**
 * Init for macros.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_macro_init(GtkBuilder *builder, app_data *data);

/**
 * Init for mouse sensor related settings.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_sensor_init(GtkBuilder* builder, app_data* data);

/**
 * @brief Load the mouse settings from disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the settings were loaded or -1 if there was an error
 */
int load_settings_from_file(app_data *data);

/**
 * @brief Save the mouse settings to disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the settings were saved or -1 if there was an error
 */
int save_settings_to_file(app_data *data);

/**
 * @brief Save the macros to disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the macros were saved or -1 if there was an error
 */
int save_macros_to_file(app_data *data);

/**
 * @brief Load the macros frp, disk.
 * 
 * @param data Application wide data structure
 * @return 0 if the macros were loaded or -1 if there was an error
 */
int load_macros_from_file(app_data *data);

/**
 * @brief Gets the menu button that has its popover shown.
 * 
 * @param data Application wide data structure
 * @return GtkMenuButton* the active menu button widget
 */
GtkMenuButton* get_active_menu_button(app_data* data);

/**
 * @brief Sets the visibility of a menu button popover
 * 
 * @param self The menu button that contains the popover
 * @param visible Whether the popover should be visible or not
 */
void menu_button_set_popover_visibility(GtkMenuButton *self, bool visible);

/**
 * @brief A method to set the stack's page.
 * 
 * @param stack The GtkStack widget
 * @param page The page number
 */
void gtk_stack_set_page(GtkStack *stack, uint32_t page);

/**
 * @brief A function switch the displayed page in a GtkStack
 * 
 * @param stack The GtkStack widget
 * @param button The GtkActionable of the button for switching the stack page
 */
void switch_stack_page(GtkStack *stack, GtkActionable* button);

/**
 * @brief Switch 
 * 
 * @param stack The main stack widget
 * @param button Unused
 */
void enter_macro_stack_page(GtkStack *stack, GtkActionable *button);

/**
 * @brief A function to disable the main stack page 
 * when inside the macro stack page
 * 
 * @param box_main The child of the main stack page
 * @param button Unused
 */
void disable_main_stack_page(GtkBox *box_main, GtkActionable *button);

/**
 * @brief Assigns a macro to a mouse button.
 * 
 * @param macro_index The index of the macro
 * @param button The button number being re-assigned
 * @param data Application wide data structure
 */
void assign_macro(uint32_t macro_index, byte button, app_data *data);

#endif
