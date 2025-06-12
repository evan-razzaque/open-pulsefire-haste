#ifndef MOUSE_BUTTONS_H
#define MOUSE_BUTTONS_H

#include <gtk/gtk.h>

#include "types.h"

/**
 * Init for mouse button remapping.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
void app_mouse_buttons_init(GtkBuilder *builder, app_data *data);

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data);

#endif