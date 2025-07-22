#include <gtk/gtk.h>
#include <stdint.h>

#include "types.h"
#include "mouse_config.h"

void gtk_stack_set_page(GtkStack *stack, uint32_t page) {
    gtk_selection_model_select_item(gtk_stack_get_pages(stack), page, TRUE);
}

int toggle_mouse_settings_visibility(app_data *data) {
    mouse_data *mouse = data->mouse;

    if (data->mouse->dev == NULL) {
        gtk_stack_set_page(
            data->widgets->stack_main,
            STACK_PAGE_DEVICE_NOT_FOUND
        );

        gtk_widget_set_sensitive(
            GTK_WIDGET(data->widgets->box_main),
            false
        );
    } else if (mouse->state == CONNECTED) {
        gtk_stack_set_page(
            data->widgets->stack_main,
            STACK_PAGE_MAIN
        );

        gtk_widget_set_sensitive(
            GTK_WIDGET(data->widgets->box_main),
            true
        );

        mouse->state = UPDATE;
    }

    return G_SOURCE_CONTINUE;
}

GtkMenuButton* get_active_menu_button(app_data *data) {
    return data->button_data.menu_button_bindings[data->button_data.selected_button];
}

void menu_button_set_popover_visibility(GtkMenuButton *self, bool is_visible) {
    if (is_visible) {
        gtk_menu_button_popup(self);
    } else {
        gtk_menu_button_popdown(self);
    }
}

void enter_macro_stack_page(GtkStack *stack, GtkActionable *button) {
    gtk_stack_set_page(stack, STACK_PAGE_MACRO);
}

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
