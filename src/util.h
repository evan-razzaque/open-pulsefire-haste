#ifndef UTIL_H
#define UTIL_H

#include <time.h>

/**
 * @brief Removes the element at `index` from `array` and decrements `length`.
 * 
 * @param array The array to remove the element from
 * @param length A variable containing the number of elements in the array
 * @param index The index of the element to remove
 */
#define array_delete_element(array, length, index) ({\
    memmove(                                         \
        (array) + (index),                           \
        (array) + (index) + 1,                       \
        ((length) - 1 - (index)) * sizeof(*array)    \
    );                                               \
    (length)--;                                      \
})

#define array_index_of(array, length, value) ({\
    int i;                                     \
    for (i = 0; i < (length); i++) {           \
        if ((array)[i] == (value)) break;      \
    }                                          \
    if (i == (length)) i = -1;                 \
    i;                                         \
})

/**
 * @brief Prints `expression` along with its value given `format`.
 */
#define printval(format, expression)\
    printf(#expression ": " format, (expression))

#define widget_add_event(builder, widget_name, detailed_signal, c_handler, data)\
	g_signal_connect(gtk_builder_get_object(builder, widget_name), detailed_signal, G_CALLBACK(c_handler), data);

/**
 * @brief Gets the clocks current time in milliseconds.
 * 
 * @return time_t the clocks time in ms
 */
time_t clock_gettime_ms();

#endif
