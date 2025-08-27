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

#ifndef MOUSE_MACRO_BUTTON_H
#define MOUSE_MACRO_BUTTON_H

#include <stdint.h>
#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE(MouseMacroButton, mouse_macro_button, MOUSE, MACRO_BUTTON, GtkBox)

#define MOUSE_TYPE_MACRO_BUTTON (mouse_macro_button_get_type())
#define MOUSE_MACRO_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MOUSE_TYPE_MACRO_BUTTON, MouseMacroButton))

/**
 * @brief Creates a MouseMacroButton for selecting a macro.
 * 
 * @param name The name of the macro
 * @param index The index of the macro
 * @return a new MouseMacroButton
 */
MouseMacroButton* mouse_macro_button_new(char* name, uint32_t index);

/**
 * @brief Sets the macro index for the MouseMacroButton.
 * 
 * @param self The MouseMacroButton instance
 * @param index The macro index
 */
void mouse_macro_button_set_index(MouseMacroButton* self, uint32_t index);

#endif
