#include <gtk/gtk.h>
#include <stdint.h>

#include "mouse_config.h"
#include "types.h"
#include "config_buttons.h"

void show_connection_lost_overlay(app_data *data) {
    GtkStack *stack_main = data->widgets->stack_main;
    GtkBox *box_main = data->widgets->box_main;
    GtkOverlay *overlay_main = data->widgets->overlay_main;

    gtk_stack_set_page(stack_main, STACK_PAGE_MAIN);
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), false);
    gtk_overlay_add_overlay(overlay_main, data->widgets->box_connection_lost);
}

void remove_connnection_lost_overlay(app_data *data) {
    GtkBox *box_main = data->widgets->box_main;
    GtkOverlay *overlay_main = data->widgets->overlay_main;
    GtkWidget *box_connection_lost = data->widgets->box_connection_lost;

    if (data->mouse->state != DISCONNECTED) gtk_widget_set_sensitive(GTK_WIDGET(box_main), true);
    if (gtk_widget_get_parent(box_connection_lost) == NULL) return;

    gtk_overlay_remove_overlay(overlay_main, box_connection_lost);
}

/**
 * @brief A function that will enable the mouse settings screen
 * when the mouse is plugged in and disable it when the mouse is unplugged.
 * 
 * @param data Application wide data structure
 * @return value indicating to leave this in the gtk main loop
 */
static void toggle_mouse_settings_visibility(app_data *data, bool show_mouse_settings) {
    if (show_mouse_settings) {
        gtk_stack_set_page(
            data->widgets->stack_main,
            STACK_PAGE_MAIN
        );
    } else {
        gtk_stack_set_page(
            data->widgets->stack_main,
            STACK_PAGE_DEVICE_NOT_FOUND
        );
    }

    gtk_widget_set_sensitive(
        GTK_WIDGET(data->widgets->box_main),
        show_mouse_settings
    );
}

void hide_mouse_settings_visibility(app_data *data) {
    remove_connnection_lost_overlay(data);
    toggle_mouse_settings_visibility(data, false);
}

void show_mouse_settings_visibility(app_data *data) {
    remove_connnection_lost_overlay(data);
    toggle_mouse_settings_visibility(data, true);
}

/**
 * @brief A function to switch to the main stack page.
 * 
 * @param stack The main stack widget
 * @param controller Unused
 */
G_MODULE_EXPORT void enter_main_stack_page(GtkStack *stack, GtkEventController *controller) {
    gtk_stack_set_page(stack, STACK_PAGE_MAIN);
}

G_MODULE_EXPORT void switch_stack_page(GtkStack *stack, GtkActionable *button) {
    gtk_stack_set_page(
        stack, 
        g_variant_get_int32(gtk_actionable_get_action_target_value(button)
    ));
}

G_MODULE_EXPORT void disable_main_stack_page(GtkBox *box_main, GtkActionable *button) {
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), false);
}

G_MODULE_EXPORT void enable_main_stack_page(GtkBox *box_main, GtkActionable *button) {
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), true);
}
