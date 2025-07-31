#ifndef MACRO_PARSER_H
#define MACRO_PARSER_H

#ifndef MACRO_PARSER_PRIVATE
#error "You cannot include this header directly"
#endif

#include "config_macro.h"

/**
 * @brief Converts a mouse macro into an array of macro events
 * that are readable by the mouse.
 * 
 * @param macro The recored macro
 * @param events The output location to store macro events into
 * @param modifier_map Used to map modifier keys to modifier bit flags for a macro key event
 * @return the number of macro events, or -1 if the macro is invalid
 */
int parse_macro(recorded_macro *macro, macro_event *events, const byte *modifier_map);

#endif