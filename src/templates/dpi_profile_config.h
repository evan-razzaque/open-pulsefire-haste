#ifndef DPI_PROFILE_CONFIG_H
#define DPI_PROFILE_CONFIG_H

#include <gtk/gtk.h>
#include <stdint.h>

struct _DpiProfileConfig {
    GtkListBoxRow parent_instance;

    GtkCheckButton *check_button;
    GtkBox *box_range_dpi_value;
    GtkRange *range_dpi_value;
    GtkSpinButton *spinner_dpi_value;
    GtkColorDialogButton *color_button_dpi_indicator;
    GtkGestureClick *gesture_click_controller;
    GtkButton *button_delete_profile;

    uint8_t profile_index;
};

G_DECLARE_FINAL_TYPE(DpiProfileConfig, dpi_profile_config, DPI, PROFILE_CONFIG, GtkListBoxRow)

#define DPI_TYPE_PROFILE_CONFIG (dpi_profile_config_get_type())
#define DPI_PROFILE_CONFIG(inst) (G_TYPE_CHECK_INSTANCE_CAST ((inst), DPI_TYPE_PROFILE_CONFIG, DpiProfileConfig))

#endif