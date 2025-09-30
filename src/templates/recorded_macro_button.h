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

#ifndef RECORDED_MACRO_BUTTON_H
#define RECORDED_MACRO_BUTTON_H

#include <stdint.h>
#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE(RecordedMacroButton, recorded_macro_button, MOUSE, MACRO_BUTTON, GtkBox)

#define RECORDED_TYPE_MACRO_BUTTON (recorded_macro_button_get_type())
#define recorded_macro_button(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), RECORDED_TYPE_MACRO_BUTTON, RecordedMacroButton))

/**
 * @brief Creates a RecordedMacroButton for selecting a macro.
 *
 * @param name The name of the macro
 * @param index The index of the macro
 * @return a new RecordedMacroButton
 */
RecordedMacroButton* recorded_macro_button_new(char* name, uint32_t index);

/**
 * @brief Sets the macro index for the RecordedMacroButton.
 *
 * @param self The RecordedMacroButton instance
 * @param index The macro index
 */
void recorded_macro_button_set_index(RecordedMacroButton* self, uint32_t index);

#endif
