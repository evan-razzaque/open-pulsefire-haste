#include <gtk/gtk.h>
#include "types.h"

#include "mouse_config.h"

GtkMenuButton* get_active_menu_button(app_data* data) {
    return data->button_data.menu_button_bindings[data->button_data.selected_button];
}

void menu_button_set_popover_visibility(GtkMenuButton *self, bool is_visible) {
    if (is_visible) {
        gtk_menu_button_popup(self);
    } else {
        gtk_menu_button_popdown(self);
    }
}

G_MODULE_EXPORT void switch_stack_page(GtkStack *stack, GtkActionable* button) {
	gtk_selection_model_select_item(
        gtk_stack_get_pages(stack),
        g_variant_get_int32(gtk_actionable_get_action_target_value(button)),
        true
    );
}

G_MODULE_EXPORT void enter_macro_stack_page(GtkBox *box_main, GtkActionable *button) {
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), false);
}

G_MODULE_EXPORT void exit_macro_stack_page(GtkBox *box_main, GtkActionable *button) {
    gtk_widget_set_sensitive(GTK_WIDGET(box_main), true);
}