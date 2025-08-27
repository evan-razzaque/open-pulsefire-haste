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

#ifndef STACK_MENU_BUTTON_BACK_H
#define STACK_MENU_BUTTON_BACK_H

#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE(StackMenuButtonBack, stack_menu_button_back, STACK, MENU_BUTTON_BACK, GtkButton)

#define STACK_TYPE_MENU_BUTTON_BACK (stack_menu_button_back_get_type())
#define STACK_MENU_BUTTON_BACK(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), STACK_TYPE_MENU_BUTTON_BACK, StackMenuButtonBack))

#endif
