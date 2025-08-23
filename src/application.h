#ifndef APPLICATION_H
#define APPLICATION_H

#include <gtk/gtk.h>

#include "device/buttons.h"
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

/**
 * @brief A function to hides and disable the mouse settings screen.
 * 
 * @param data Application wide data structure
 */
void hide_mouse_settings_visibility(app_data *data);

/**
 * @brief A function to show and enable the mouse settings screen.
 * 
 * @param data Application wide data structure
 */
void show_mouse_settings_visibility(app_data *data);

/**
 * @brief A function switch the displayed page in a GtkStack
 * 
 * @param stack The GtkStack widget
 * @param button The GtkActionable of the button for switching the stack page
 */
void switch_stack_page(GtkStack *stack, GtkActionable* button);

/**
 * @brief A function to update the label of a menu button for a mouse button.
 * 
 * @param button The index of the mouse button
 * @param action The new action of the mouse button
 * @param data Application wide data structure
 */
void update_menu_button_label(MOUSE_BUTTON button, uint16_t action, app_data *data);

/**
 * @brief Sets the settings for the mouse.
 * 
 * @param data Application wide data structure
 */
void load_mouse_settings(app_data *data);

/**
 * @brief Saves the settings to the mouse.
 * 
 * @param self The save button
 * @param mouse mouse_data instance
 */
void save_mouse_settings(GtkWidget *self, mouse_data *mouse);

/**
 * @brief A function to setup and activate the application.
 * 
 * @param app GtkApplication instance
 * @param data Application wide data structure
 */
void activate(GtkApplication *app, app_data *data);

#endif