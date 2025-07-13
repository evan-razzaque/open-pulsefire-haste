#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define printval(val, format_specifier, additional_string)\
    printf(#val ": " format_specifier additional_string, (val))

#endif