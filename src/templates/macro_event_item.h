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

#ifndef MACRO_EVENT_ITEM_H
#define MACRO_EVENT_ITEM_H

#include <gtk/gtk.h>
#include <stdint.h>

#include "config_macro.h"

G_DECLARE_FINAL_TYPE(MacroEventItem, macro_event_item, MACRO, EVENT_ITEM, GtkBox)

#define MACRO_TYPE_EVENT_ITEM (macro_event_item_get_type())
#define MACRO_EVENT_ITEM(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MACRO_TYPE_EVENT_ITEM, MacroEventItem))

/**
 * @brief Creates a MacroEventItem.
 * 
 * @param action_name The name of the action.
 * @param delay The delay of this action
 * @param event_type A MACRO_EVENT_TYPE value
 * @param event_index The index of the macro event
 * @return a new MacroEventItem
 */
MacroEventItem* macro_event_item_new(const char* action_name, uint16_t delay, MACRO_EVENT_TYPE event_type, int event_index);

#endif
