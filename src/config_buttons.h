#ifndef CONFIG_BUTTONS_H
#define CONFIG_BUTTONS_H

#include <stdint.h>
#include <gtk/gtk.h>
#include "types.h"

/**
 * Init for mouse button remapping.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_config_buttons_init(GtkBuilder *builder, app_data *data);

/**
 * @brief Stores the menu buttons used to re-assign each mouse button.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void store_action_menu_buttons(GtkBuilder *builder, app_data *data);

/**
 * @brief Set the menu buttons model object
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param menu_buttons The menu buttons
 *
 */
void set_menu_buttons_model(GtkBuilder *builder, GtkMenuButton *menu_buttons[]);

/**
 * Adds all button action values from the SIMPLE_MOUSE_ACTION enum (see device/buttons.h),
 * for access via dropdown item indicies.
 * 
 * @param action_values Action values for the mouse buttons
 */
void add_action_values(mouse_action_values *action_values);

/**
 * Testing function for keyboard events.
 */
int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data);

void set_mouse_button(GtkMenuButton *self, GParamSpec *param_spec, app_data *data);

/**
 * Remap mouse button to a SIMPLE_MOUSE_ACTION (see device/buttons.h).
 * 
 * @param action The action instance
 * @param mapping_data GVariant instance containing the activated menu item's target value.
 * The value is a hexdecimal number as a string, containing the action type and action value.
 * Format (in hex bytes): <type_index> <value_index>
 * @param data Application wide data structure
 */
void change_mouse_binding(GSimpleAction *action, GVariant *mapping_data, app_data *data);

#endif