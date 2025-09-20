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