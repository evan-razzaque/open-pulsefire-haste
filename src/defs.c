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

#include <time.h>
#include "defs.h"

time_t clock_gettime_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    
    return (t.tv_sec * 1000) + (t.tv_nsec / (1000 * 1000));
}

void gtk_spin_button_hide_buttons(GtkSpinButton *self) {
    GtkWidget *button_decrease_value = gtk_widget_get_next_sibling(
        gtk_widget_get_first_child(GTK_WIDGET(self))
    );
    GtkWidget *button_increase_value = gtk_widget_get_next_sibling(button_decrease_value);
    
    gtk_widget_set_visible(button_decrease_value, false);
    gtk_widget_set_visible(button_increase_value, false);
}

