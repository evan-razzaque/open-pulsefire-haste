/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

#include <gtk/gtk.h>

#include "stack_menu_button_back.h"

struct _StackMenuButtonBack {
    GtkButton parent_instance;

    GtkLabel *label;
    GtkImage *image;
};

G_DEFINE_TYPE(StackMenuButtonBack, stack_menu_button_back, GTK_TYPE_BUTTON)

static void stack_menu_button_back_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), STACK_TYPE_MENU_BUTTON_BACK);
    G_OBJECT_CLASS(stack_menu_button_back_parent_class)->dispose(gobject);
}

static void stack_menu_button_back_class_init(StackMenuButtonBackClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = stack_menu_button_back_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/stack-menu-button-back.ui");

    gtk_widget_class_bind_template_child(widget_class, StackMenuButtonBack, label);
    gtk_widget_class_bind_template_child(widget_class, StackMenuButtonBack, image);
}

static void stack_menu_button_back_init(StackMenuButtonBack *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}
