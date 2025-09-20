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
#include <stdio.h>
#include <stdint.h>

#include "macro_event_item.h"
#include "device/buttons.h"
#include "defs.h"

#define ICON_DOWN_ARROW "go-down"
#define ICON_UP_ARROW "go-up"

struct _MacroEventItem {
    GtkBox parent_instance;
    GtkLabel *label_action_name;
    GtkSpinButton *spin_button_delay;
    GtkImage *image_action_type;
    int event_index;
    uint16_t delay;
};

G_DEFINE_TYPE(MacroEventItem, macro_event_item, GTK_TYPE_BOX)

enum {
    DELAY_CHANGED,
    N_SIGNALS
};

static guint signals[N_SIGNALS] = {0};

/**
 * @brief A function to signal when the delay value changes via the `delay-changed` signal.
 *
 * @param self The MacroEventItem instance
 * @param spin_button_delay The spin button delay widget
 */
static void change_delay_value(MacroEventItem *self, GtkSpinButton *spin_button_delay) {
    uint16_t new_delay = gtk_spin_button_get_value_as_int(spin_button_delay);
    self->delay = new_delay;

    g_signal_emit(self, signals[DELAY_CHANGED], 0, self->event_index, new_delay);
}

static void macro_event_item_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), MACRO_TYPE_EVENT_ITEM);
    G_OBJECT_CLASS(macro_event_item_parent_class)->dispose(gobject);
}

static void macro_event_item_class_init(MacroEventItemClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = macro_event_item_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/open_pulsefire_haste/macro-event-item.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);

    signals[DELAY_CHANGED] = g_signal_new(
        "delay-changed",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE,
        2, G_TYPE_INT, G_TYPE_INT
    );

    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, label_action_name);
    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, spin_button_delay);
    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, image_action_type);

    gtk_widget_class_bind_template_callback(widget_class, change_delay_value);
}

static void macro_event_item_init(MacroEventItem *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MacroEventItem* macro_event_item_new(const char* action_name, uint16_t delay, MACRO_EVENT_TYPE event_type, int event_index) {
    MacroEventItem *self = g_object_new(MACRO_TYPE_EVENT_ITEM, NULL);
    self->event_index = event_index;
    self->delay = delay;

    if (delay > 0) {
        gtk_spin_button_hide_buttons(self->spin_button_delay);
        gtk_spin_button_set_value(self->spin_button_delay, delay);
    } else {
        gtk_widget_unparent(GTK_WIDGET(self->spin_button_delay));
    }

    gtk_label_set_text(self->label_action_name, action_name);

    gtk_image_set_from_icon_name(
        self->image_action_type,
        (event_type == MACRO_EVENT_TYPE_DOWN)? ICON_DOWN_ARROW : ICON_UP_ARROW
    );

    return self;
}
