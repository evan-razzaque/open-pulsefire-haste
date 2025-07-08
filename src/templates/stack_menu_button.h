#ifndef STACK_MENU_BUTTON_H
#define STACK_MENU_BUTTON_H

#include <gtk/gtk.h>

struct _StackMenuButton {
    GtkButton parent_instance;

    GtkLabel *label;
    GtkImage *image;
};

G_DECLARE_FINAL_TYPE(StackMenuButton, stack_menu_button, STACK, MENU_BUTTON, GtkButton)

#define STACK_TYPE_MENU_BUTTON (stack_menu_button_get_type())
#define STACK_MENU_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST ((inst), STACK_TYPE_MENU_BUTTON, StackMenuButton))

#endif