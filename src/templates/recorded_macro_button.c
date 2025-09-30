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
#include "recorded_macro_button.h"

struct _RecordedMacroButton {
    GtkBox parent_instance;

    GtkButton *button_name;
    GtkButton *button_edit;
    GtkButton *button_delete;
};

G_DEFINE_TYPE(RecordedMacroButton, recorded_macro_button, GTK_TYPE_BOX)

static void recorded_macro_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), RECORDED_TYPE_MACRO_BUTTON);
    G_OBJECT_CLASS(recorded_macro_button_parent_class)->dispose(gobject);
}

static void recorded_macro_button_class_init(RecordedMacroButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = recorded_macro_button_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/open_pulsefire_haste/recorded-macro-button.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);

    gtk_widget_class_bind_template_child(widget_class, RecordedMacroButton, button_name);
    gtk_widget_class_bind_template_child(widget_class, RecordedMacroButton, button_edit);
    gtk_widget_class_bind_template_child(widget_class, RecordedMacroButton, button_delete);
}

static void recorded_macro_button_init(RecordedMacroButton *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

RecordedMacroButton* recorded_macro_button_new(char *name, uint32_t index) {
    RecordedMacroButton *self = g_object_new(RECORDED_TYPE_MACRO_BUTTON, "name", name, NULL);

    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_name), (const char*) G_VARIANT_TYPE_UINT32, index);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_delete), (const char*) G_VARIANT_TYPE_UINT32, index);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_edit), (const char*) G_VARIANT_TYPE_UINT32, index);

    return self;
}

void recorded_macro_button_set_index(RecordedMacroButton* self, uint32_t index) {
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_name), (const char*) G_VARIANT_TYPE_UINT32, index);
    gtk_actionable_set_action_target(GTK_ACTIONABLE(self->button_delete), (const char*) G_VARIANT_TYPE_UINT32, index);
}
