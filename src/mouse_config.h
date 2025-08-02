#ifndef MOUSE_CONFIG_H
#define MOUSE_CONFIG_H

#include <gtk/gtk.h>
#include "types.h"

enum {
    STACK_PAGE_MAIN,
    STACK_PAGE_MACRO,
    STACK_PAGE_DEVICE_NOT_FOUND
};

/**
 * @brief A function that will enable the mouse settings screen
 * when the mouse is plugged in and disable it when the mouse is unplugged.
 * 
 * @param data Application wide data structure
 * @return value indicating to leave this in the gtk main loop
 */
int toggle_mouse_settings_visibility(app_data *data);

/**
 * @brief A function switch the displayed page in a GtkStack
 * 
 * @param stack The GtkStack widget
 * @param button The GtkActionable of the button for switching the stack page
 */
void switch_stack_page(GtkStack *stack, GtkActionable* button);

#endif
