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

#include "profile_button.h"
#include "defs.h"

struct _ProfileButton {
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
    SELECT_PROFILE,
    RENAME_PROFILE_NAME,
    DELETE_PROFILE,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

G_DEFINE_TYPE(ProfileButton, profile_button, GTK_TYPE_BOX)

/**
 * @brief A function to change the profile button display name.
 * If the new display name is different and valid, the `rename-profile` signal
 * will be emitted.
 *
 * @param self The ProfileButton instance
 * @param name The new display name
 */
static void rename_profile(ProfileButton *self, const char *name) {
    debug("self->name: %p, name: %p\n", self->name, name);
    if (strcmp(self->name, name) == 0) return;

    if (!is_valid_profile_name(name)) {
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
    self->name = gtk_button_get_label(self->button_name);
}

/**
 * @brief A function to toggle editing for the profile button name.
 *
 * @param self The ProfileButton instance
 * @param is_editing_name Whether or not the name is being edited or not
 */
static void toggle_name_editing(ProfileButton *self, bool is_editing_name) {
    self->is_editing_name = is_editing_name;

    gtk_widget_set_visible(GTK_WIDGET(self->button_name), !self->is_editing_name);
    gtk_widget_set_visible(GTK_WIDGET(self->editable_label_name), self->is_editing_name);
    gtk_widget_set_visible(GTK_WIDGET(self->button_cancel_edit), self->is_editing_name);
}

/**
 * @brief Cancels editing the profile button name.
 *
 * @param self The ProfileButton instance
 * @param button Unused
 */
static void cancel_name_edit(ProfileButton *self, GtkButton *button) {
    toggle_name_editing(self, false);
    gtk_editable_label_stop_editing(self->editable_label_name, true);
}

/**
 * @brief Handles editing state changes with the `ProfileButton->editable_label_name`.
 *
 * @param self The ProfileButton instance
 * @param param Unused
 * @param editable_label The editable label widget
 */
static void update_editing_state(ProfileButton *self, GParamSpec *param, GtkEditableLabel *editable_label) {
    self->is_editing_name = gtk_editable_label_get_editing(self->editable_label_name);

    if (!self->is_editing_name) {
        toggle_name_editing(self, false);

        debug("self->name: %p, name: %p\n", self->name, gtk_editable_get_text(GTK_EDITABLE(self->editable_label_name)));
        rename_profile(self, gtk_editable_get_text(GTK_EDITABLE(self->editable_label_name)));
    }
}

/**
 * @brief A function to start/stop editing of the profile button name.
 *
 * @param self The ProfileButton instance
 * @param button Unused
 */
static void edit_profile_name(ProfileButton *self, GtkButton *button) {
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

/**
 * @brief Emits `select-profile` when the profile name is clicked.
 *
 * @param self The ProfileButton instance
 * @param button Unused
 */
static void select_profile(ProfileButton *self, GtkButton *button) {
    g_signal_emit(
        self,
        signals[SELECT_PROFILE],
        0,
        self->name
    );
}

/**
 * @brief Emits `delete-profile` when the delete button is clicked.
 *
 * @param self The ProfileButton instance
 * @param button Unused
 */
static void delete_profile(ProfileButton *self, GtkButton *button) {
    bool delete_success;

    g_signal_emit(
        self,
        signals[DELETE_PROFILE],
        0,
        self->name,
        &delete_success
    );

    if (!delete_success) return;
    gtk_widget_unparent(GTK_WIDGET(self));
}

static void profile_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), PROFILE_TYPE_BUTTON);
    G_OBJECT_CLASS(profile_button_parent_class)->dispose(gobject);
}

static void profile_button_class_init(ProfileButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = profile_button_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/open_pulsefire_haste/profile-button.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);

    signals[SELECT_PROFILE] = g_signal_new(
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

    signals[DELETE_PROFILE] = g_signal_new(
        "delete-profile",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_BOOLEAN,
        1, G_TYPE_STRING
    );

    gtk_widget_class_bind_template_child(widget_class, ProfileButton, editable_label_name);
    gtk_widget_class_bind_template_child(widget_class, ProfileButton, button_name);
    gtk_widget_class_bind_template_child(widget_class, ProfileButton, button_edit);
    gtk_widget_class_bind_template_child(widget_class, ProfileButton, button_cancel_edit);
    gtk_widget_class_bind_template_child(widget_class, ProfileButton, button_delete);

    // Buttons
    gtk_widget_class_bind_template_callback(widget_class, select_profile);
    gtk_widget_class_bind_template_callback(widget_class, edit_profile_name);
    gtk_widget_class_bind_template_callback(widget_class, cancel_name_edit);
    gtk_widget_class_bind_template_callback(widget_class, delete_profile);

    // Editable Label
    gtk_widget_class_bind_template_callback(widget_class, update_editing_state);
}

static void profile_button_init(ProfileButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

ProfileButton* profile_button_new(const char *name, bool is_default_profile) {
    ProfileButton *self = g_object_new(PROFILE_TYPE_BUTTON, "name", name, NULL);
    self->name = gtk_button_get_label(self->button_name);

    gtk_widget_set_sensitive(GTK_WIDGET(self->button_edit), !is_default_profile);
    gtk_widget_set_sensitive(GTK_WIDGET(self->button_delete), !is_default_profile);

    return self;
}