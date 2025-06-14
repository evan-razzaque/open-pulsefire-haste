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
 * Testing function for keyboard events.
 */
int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data);

/**
 * @brief Sets the mouse button to be re-assigned.
 * 
 * @param self The menu button objecy
 * @param param_spec Unused
 * @param data Application wide data structure
 */
void set_mouse_button(GtkMenuButton *self, GParamSpec *param_spec, app_data *data);

/**
 * Remap mouse button to a SIMPLE_MOUSE_ACTION (see device/buttons.h).
 * 
 * @param action The action instance
 * @param mapping_data GVariant instance containing the activated menu item's target value.
 * The value is a string containing a SIMPLE_MOUSE_ACTION (see device/buttons.h) and the name of the action, seperated by a '|'.
 * @param data Application wide data structure
 */
void change_mouse_binding(GSimpleAction *action, GVariant *mapping_data, app_data *data);

#endif