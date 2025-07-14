#ifndef DPI_PROFILE_CONFIG_H
#define DPI_PROFILE_CONFIG_H

#include <gtk/gtk.h>
#include <stdint.h>

G_DECLARE_FINAL_TYPE(DpiProfileConfig, dpi_profile_config, DPI, PROFILE_CONFIG, GtkListBoxRow)

#define DPI_TYPE_PROFILE_CONFIG (dpi_profile_config_get_type())
#define DPI_PROFILE_CONFIG(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), DPI_TYPE_PROFILE_CONFIG, DpiProfileConfig))

DpiProfileConfig* dpi_profile_config_new(GtkCheckButton *check_button_group, uint8_t profile_index);

uint16_t dpi_profile_config_get_dpi_value(DpiProfileConfig *self);

const GdkRGBA* dpi_profile_config_get_indicator(DpiProfileConfig *self);

void dpi_profile_config_set_index(DpiProfileConfig *self, uint8_t profile_index);

void dpi_profile_config_set_dpi_value(DpiProfileConfig *self, uint16_t dpi_value);

void dpi_profile_config_set_indicator(DpiProfileConfig *self, GdkRGBA *rgb);

void dpi_profile_config_activate(DpiProfileConfig *self);

void dpi_profile_config_delete_button_set_enabled(DpiProfileConfig *self, bool enabled);

void dpi_profile_config_remove_check_button_group(DpiProfileConfig *self);

#endif