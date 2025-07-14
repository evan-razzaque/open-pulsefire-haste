#ifndef STACK_MENU_BUTTON_BACK_H
#define STACK_MENU_BUTTON_BACK_H

#include <gtk/gtk.h>

struct _StackMenuButtonBack {
    GtkButton parent_instance;

    GtkLabel *label;
    GtkImage *image;
};

G_DECLARE_FINAL_TYPE(StackMenuButtonBack, stack_menu_button_back, STACK, MENU_BUTTON_BACK, GtkButton)

#define STACK_TYPE_MENU_BUTTON_BACK (stack_menu_button_back_get_type())
#define STACK_MENU_BUTTON_BACK(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), STACK_TYPE_MENU_BUTTON_BACK, StackMenuButtonBack))

#endif