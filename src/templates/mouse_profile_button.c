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
#include "mouse_profile_button.h"

struct _MouseProfileButton {
    GtkBox parent_instance;
    
    GtkButton *button_name; 
    GtkButton *button_edit;
    GtkButton *button_delete;
};

enum {
    SELECT_MOUSE_PROFILE,
    EDIT_MOUSE_PROFILE,
    DELETE_MOUSE_PROFILE,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

G_DEFINE_TYPE(MouseProfileButton, mouse_profile_button, GTK_TYPE_BOX)

static void mouse_profile_selected(MouseProfileButton *self, GtkButton *button) {
    g_signal_emit(
        self,
        signals[SELECT_MOUSE_PROFILE],
        0,
        gtk_button_get_label(button)
    );
}

static void mouse_profile_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), MOUSE_TYPE_PROFILE_BUTTON);
    G_OBJECT_CLASS(mouse_profile_button_parent_class)->dispose(gobject);
}

static void mouse_profile_button_class_init(MouseProfileButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = mouse_profile_button_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/open_pulsefire_haste/mouse-profile-button.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);

    signals[SELECT_MOUSE_PROFILE] = g_signal_new(
        "select-profile",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE,
        1, G_TYPE_STRING
    );
    
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_name);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_edit);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_delete);

    gtk_widget_class_bind_template_callback(widget_class, mouse_profile_selected);
}

static void mouse_profile_button_init(MouseProfileButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MouseProfileButton* mouse_profile_button_new(char *name, bool is_default_profile) {
    MouseProfileButton *self = g_object_new(MOUSE_TYPE_PROFILE_BUTTON, "name", name, NULL);
    
    gtk_widget_set_visible(GTK_WIDGET(self->button_edit), !is_default_profile);
    gtk_widget_set_visible(GTK_WIDGET(self->button_delete), !is_default_profile);
    
    return self;
}

// TODO: Convert to signal
void mouse_profile_button_set_selection_callback(MouseProfileButton *self, void (*callback)(GtkButton*, void *user_data), void *user_data) {
    g_signal_connect(self->button_name, "clicked", G_CALLBACK(callback), user_data);
}