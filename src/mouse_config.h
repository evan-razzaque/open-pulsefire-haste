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
 * @brief Displays an overlay when the connection to the mouse
 * is lost or the mouse goes to sleep (wireless only).
 * 
 * @param data Application wide data structure
 */
void show_connection_lost_overlay(app_data *data);

/**
 * @brief Removes the overlay displayed by `show_connection_lost_overlay`
 * when the mouse connection is found or the mouse wakes up.
 * 
 * @param data Application wide data structure
 */
void remove_connnection_lost_overlay(app_data *data);

void hide_mouse_settings_visibility(app_data *data);
void show_mouse_settings_visibility(app_data *data);

/**
 * @brief A function switch the displayed page in a GtkStack
 * 
 * @param stack The GtkStack widget
 * @param button The GtkActionable of the button for switching the stack page
 */
void switch_stack_page(GtkStack *stack, GtkActionable* button);

#endif
