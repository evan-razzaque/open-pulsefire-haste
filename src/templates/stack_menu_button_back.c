#include <gtk/gtk.h>
#include <string.h>

#include "stack_menu_button_back.h"

G_DEFINE_TYPE(StackMenuButtonBack, stack_menu_button_back, GTK_TYPE_BUTTON)

static void stack_menu_button_back_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), STACK_TYPE_MENU_BUTTON_BACK);
    G_OBJECT_CLASS(stack_menu_button_back_parent_class)->dispose(gobject);
}

static void stack_menu_button_back_class_init(StackMenuButtonBackClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = stack_menu_button_back_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/com/haste/stack-menu-button-back.ui");

    gtk_widget_class_bind_template_child(widget_class, StackMenuButtonBack, label);
    gtk_widget_class_bind_template_child(widget_class, StackMenuButtonBack, image);
}

static void stack_menu_button_back_init(StackMenuButtonBack *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}