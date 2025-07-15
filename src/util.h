#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>

#define printval(val, format_specifier, additional_string)\
    printf(#val ": " format_specifier additional_string, (val))

/**
 * @brief Removes the element at `index` from `array` and decrements `length`.
 * 
 * @param array The array to remove the element from
 * @param length A variable containing the number of elements in the array
 * @param index The index of the element to remove
 */
#define array_delete_element(array, length, index) ({\
    memmove(\
        (array) + (index),\
        (array) + (index) + 1,\
        ((length) - 1 - (index)) * sizeof(*array)\
    );\
    (length)--;\
})

#endif
