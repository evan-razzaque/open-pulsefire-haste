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

#ifndef PROFILE_BUTTON_H
#define PROFILE_BUTTON_H

#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE(ProfileButton, profile_button, MOUSE, PROFILE_BUTTON, GtkBox)

#define PROFILE_TYPE_BUTTON (profile_button_get_type())
#define PROFILE_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), PROFILE_TYPE_BUTTON, ProfileButton))

/**
 * @brief Creates a ProfileButton.
 *
 * @param name The name of the profile
 * @param is_default_profile Whether the profile is the default profile or not
 * @return a new ProfileButton
 */
ProfileButton* profile_button_new(const char *name, bool is_default_profile);

#endif