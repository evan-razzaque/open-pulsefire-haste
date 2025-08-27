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

#ifndef DPI_PROFILE_CONFIG_H
#define DPI_PROFILE_CONFIG_H

#include <gtk/gtk.h>
#include <stdint.h>

G_DECLARE_FINAL_TYPE(DpiProfileConfig, dpi_profile_config, DPI, PROFILE_CONFIG, GtkListBoxRow)

#define DPI_TYPE_PROFILE_CONFIG (dpi_profile_config_get_type())
#define DPI_PROFILE_CONFIG(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), DPI_TYPE_PROFILE_CONFIG, DpiProfileConfig))

/**
 * @brief Emitted when the one of the values in the dpi profile row changes.
 * 
 * @param self The DpiProfileConfig instance
 * @param profile_index The index of the dpi profile
 * @param dpi_value The updated dpi value
 * @param indicator The updated color indicator
 * @param user_data User data to pass to the signal callback
 */
void profile_updated(
    DpiProfileConfig *self,
    uint8_t profile_index,
    int dpi_value,
    GdkRGBA *indicator,
    gpointer *user_data
);

/**
 * @brief Creates a DpiProfileConfig.
 * 
 * @param check_button_group The GtkButtonGroup for dpi profile config
 * @param profile_index The index of the dpi profile
 * @return a new DpiProfileConfig
 */
DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group, uint8_t profile_index);

/**
 * @brief Gets the dpi value.
 * 
 * @param self The DpiProfileConfig instance
 * @return The dpi value of the instance
 */
uint16_t dpi_profile_config_get_dpi_value(DpiProfileConfig *self);

/**
 * @brief Get the color indicator.
 * 
 * @param self The DpiProfileConfig instance
 * @return a GdkRGBA instance
 */
const GdkRGBA* dpi_profile_config_get_indicator(DpiProfileConfig *self);

/**
 * @brief Set the profile index for the DpiProfileConfig.
 * 
 * @param self The DpiProfileConfig instance
 * @param profile_index The profile index
 */
void dpi_profile_config_set_index(DpiProfileConfig *self, uint8_t profile_index);

/**
 * @brief Set the dpi value for the DpiProfileConfig.
 * 
 * @param self The DpiProfileConfig instance
 * @param dpi_value The dpi value
 */
void dpi_profile_config_set_dpi_value(DpiProfileConfig *self, uint16_t dpi_value);

/**
 * @brief Set the indicator for the DpiProfileConfig.
 * 
 * @param self The DpiProfileConfig instance
 * @param rgb A GdkRGBA instance for the color indicator
 */
void dpi_profile_config_set_indicator(DpiProfileConfig *self, GdkRGBA *rgb);

/**
 * @brief A function to select the dpi profile associated with the DpiProfileConfig.
 * 
 * @param self The DpiProfileConfig instance
 */
void dpi_profile_config_activate(DpiProfileConfig *self);

/**
 * @brief Sets whether the DpiProfileConfig's delete button is enabled or not.
 * 
 * @param self The DpiProfileConfig instance 
 * @param enabled The enabled value to set
 */
void dpi_profile_config_delete_button_set_enabled(DpiProfileConfig *self, bool enabled);

/**
 * @brief Removes the check button group from the DpiProfileConfig
 * 
 * @param self The DpiProfileConfig instance
 */
void dpi_profile_config_remove_check_button_group(DpiProfileConfig *self);

#endif
