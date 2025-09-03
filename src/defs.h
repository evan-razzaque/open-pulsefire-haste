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

#ifndef DEFS_H
#define DEFS_H

#include <time.h>
#include <gtk/gtk.h>

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#define PROFILE_NAME_MAX_LENGTH (64)

#define BOOL_STR(condition) (((condition)) ? "true" : "false")

/**
 * @brief Removes the element at `index` from `array` and decrements `length`.
 * 
 * @param array The array to remove the element from
 * @param length A variable containing the number of elements in the array
 * @param index The index of the element to remove
 */
#define array_delete_element(array, length, index) ({ \
    memmove(                                          \
        (array) + (index),                            \
        (array) + (index) + 1,                        \
        ((length) - 1 - (index)) * sizeof((array)[0]) \
    );                                                \
    (length)--;                                       \
})

#define array_index_of(array, length, value) ({ \
    int i;                                      \
    for (i = 0; i < (length); i++) {            \
        if ((array)[i] == (value)) break;       \
    }                                           \
    if (i == (length)) i = -1;                  \
    i;                                          \
})

#define sleep_ms(milliseconds) g_usleep(1000 * (milliseconds))

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data) \
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

/**
 * @brief Prints `expression` along with its value given `format`.
 */
#define printval(format, expression) \
    printf(#expression ": " format, (expression))

#define debug(...) printf(__FILE__ ":" stringify(__LINE__) ": " __VA_ARGS__)

#define stringify(x) stringify0(x)
#define stringify0(x) #x

/**
 * @brief Gets current time in milliseconds from the clock `CLOCK_MONOTONIC`.
 * 
 * @return time_t the clocks time in ms
 */
time_t clock_gettime_ms();

/**
 * @brief A function to hide the buttons for a GtkSpinButton widget.
 * 
 * @param self The GtkSpinButton instance
 */
void gtk_spin_button_hide_buttons(GtkSpinButton *self);

#endif
