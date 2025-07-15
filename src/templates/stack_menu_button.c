#include <gtk/gtk.h>
#include <string.h>

#include "stack_menu_button.h"

G_DEFINE_TYPE(StackMenuButton, stack_menu_button, GTK_TYPE_BUTTON)

static void stack_menu_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), STACK_TYPE_MENU_BUTTON);
    G_OBJECT_CLASS(stack_menu_button_parent_class)->dispose(gobject);
}

static void stack_menu_button_class_init(StackMenuButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = stack_menu_button_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/stack-menu-button.ui");

    gtk_widget_class_bind_template_child(widget_class, StackMenuButton, label);
    gtk_widget_class_bind_template_child(widget_class, StackMenuButton, image);
}

static void stack_menu_button_init(StackMenuButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}
