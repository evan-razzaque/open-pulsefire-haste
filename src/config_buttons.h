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
 * Adds all button action values from the MOUSE_ACTION enum (see device/buttons.h),
 * for access via dropdown item indicies.
 * 
 * @param action_values Action values for the mouse buttons
 */
void add_action_values(mouse_action_values *action_values);

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data);

void print_selected_item(GtkDropDown *self, GParamSpec *param_spec, void* data);

#endif