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

#ifndef MOUSE_PROFILE_BUTTON_H
#define MOUSE_PROFILE_BUTTON_H

#include <gtk/gtk.h>
#include <application.h>

G_DECLARE_FINAL_TYPE(MouseProfileButton, mouse_profile_button, MOUSE, PROFILE_BUTTON, GtkBox)

#define MOUSE_TYPE_PROFILE_BUTTON (mouse_profile_button_get_type())
#define MOUSE_PROFILE_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MOUSE_TYPE_PROFILE_BUTTON, MouseProfileButton))

/**
 * @brief Creates a MouseProfileButton.
 * 
 * @param name The name of the mouse profile
 * @param is_default_profile Whether the mouse profile is the default profile or not
 * @return a new MouseProfileButton
 */
MouseProfileButton* mouse_profile_button_new(char *name, bool is_default_profile);

/**
 * @brief Sets the callback for the MouseProfileButton being selected.
 * 
 * @param self The MouseProfileButton instance
 * @param callback The callback to be called
 * @param user_data user data to pass to the callback
 */
void mouse_profile_button_set_selection_callback(MouseProfileButton *self, void (*callback)(GtkButton*, void *user_data), void *user_data);

#endif