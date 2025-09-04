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
#include "defs.h"

struct _MouseProfileButton {
    GtkBox parent_instance;
    
    GtkEditableLabel *editable_label_name;
    GtkButton *button_name; 
    GtkButton *button_edit;
    GtkButton *button_cancel_edit;
    GtkButton *button_delete;

    bool is_editing_name;
    const char *name;
};

enum {
    SELECT_MOUSE_PROFILE,
    RENAME_PROFILE_NAME,
    DELETE_MOUSE_PROFILE,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

G_DEFINE_TYPE(MouseProfileButton, mouse_profile_button, GTK_TYPE_BOX)

static void rename_profile(MouseProfileButton *self, const char *name) {
    if (strcmp(self->name, name) == 0) return;

    if (
        strstr(name, "..") != NULL
        || strstr(name, PATH_SEP) != NULL
        || strlen(name) > PROFILE_NAME_MAX_LENGTH
    ) {
        gtk_editable_set_text(GTK_EDITABLE(self->editable_label_name), self->name);
        return;
    }
    
    bool rename_successful;

    g_signal_emit(
        self,
        signals[RENAME_PROFILE_NAME],
        0,
        self->name,
        name,
        &rename_successful
    );

    if (!rename_successful) return;

    gtk_button_set_label(self->button_name, name);
    self->name = name;
}

static void toggle_name_editing(MouseProfileButton *self, bool is_editing_name) {
    self->is_editing_name = is_editing_name;

    gtk_widget_set_visible(GTK_WIDGET(self->button_name), !self->is_editing_name);
    gtk_widget_set_visible(GTK_WIDGET(self->editable_label_name), self->is_editing_name);
    gtk_widget_set_visible(GTK_WIDGET(self->button_cancel_edit), self->is_editing_name);
}

static void cancel_name_edit(MouseProfileButton *self, GtkButton *button) {
    toggle_name_editing(self, false);
    gtk_editable_label_stop_editing(self->editable_label_name, true);
}

/**
 * @brief Handles editing state changes with the profile name editable label.
 * 
 * @param self The MouseProfileButton instance
 * @param param Unused
 * @param editable_label_name The editable label widget 
 */
static void update_editing_state(MouseProfileButton *self, GParamSpec *param, GtkEditableLabel *editable_label_name) {
    self->is_editing_name = gtk_editable_label_get_editing(self->editable_label_name);

    if (!self->is_editing_name) {
        toggle_name_editing(self, false);
        rename_profile(self, gtk_editable_get_text(GTK_EDITABLE(self->editable_label_name)));
    }

    debug("Profile Name: %p, Editing: %s\n", gtk_editable_get_text(GTK_EDITABLE(self->editable_label_name)), BOOL_STR(self->is_editing_name));
}

static void edit_profile_name(MouseProfileButton *self, GtkButton *button) {
    toggle_name_editing(self, !self->is_editing_name);

    if (self->is_editing_name) {
        gtk_editable_set_text(
            GTK_EDITABLE(self->editable_label_name),
            gtk_button_get_label(self->button_name)
        );
        gtk_editable_label_start_editing(self->editable_label_name);
    } else {
        gtk_editable_label_stop_editing(self->editable_label_name, true);
    }
}

static void select_profile(MouseProfileButton *self, GtkButton *button) {
    g_signal_emit(
        self,
        signals[SELECT_MOUSE_PROFILE],
        0,
        self->name
    );
}

static void delete_profile(MouseProfileButton *self, GtkButton *button) {
    bool delete_success;

    g_signal_emit(
        self,
        signals[DELETE_MOUSE_PROFILE],
        0,
        self->name,
        &delete_success
    );

    if (!delete_success) return;
    gtk_widget_unparent(GTK_WIDGET(self));
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

    signals[RENAME_PROFILE_NAME] = g_signal_new(
        "rename-profile",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_BOOLEAN,
        2, G_TYPE_STRING, G_TYPE_STRING
    );

    signals[DELETE_MOUSE_PROFILE] = g_signal_new(
        "delete-profile",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_BOOLEAN,
        1, G_TYPE_STRING
    );
    
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, editable_label_name);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_name);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_edit);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_cancel_edit);
    gtk_widget_class_bind_template_child(widget_class, MouseProfileButton, button_delete);

    // Buttons
    gtk_widget_class_bind_template_callback(widget_class, select_profile);
    gtk_widget_class_bind_template_callback(widget_class, edit_profile_name);
    gtk_widget_class_bind_template_callback(widget_class, cancel_name_edit);
    gtk_widget_class_bind_template_callback(widget_class, delete_profile);

    // Editable Label
    gtk_widget_class_bind_template_callback(widget_class, update_editing_state);
}

static void mouse_profile_button_init(MouseProfileButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MouseProfileButton* mouse_profile_button_new(const char *name, bool is_default_profile) {
    MouseProfileButton *self = g_object_new(MOUSE_TYPE_PROFILE_BUTTON, "name", name, NULL);
    self->name = gtk_button_get_label(self->button_name);
    
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_edit), !is_default_profile);
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_delete), !is_default_profile);
    
    return self;
}