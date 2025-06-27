#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "device/mouse.h"

#include "types.h"
#include "mouse_config.h"

static void change_polling_rate(GtkCheckButton* self, app_data *data) {
    if (!gtk_check_button_get_active(self)) return;
    byte polling_rate_value = atoi(gtk_label_get_text(GTK_LABEL(gtk_widget_get_next_sibling(GTK_WIDGET(self)))));
    
    set_polling_rate(data->mouse->dev, polling_rate_value);
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    widget_add_event(builder, "pollingRate0", "toggled", change_polling_rate, data);
    widget_add_event(builder, "pollingRate1", "toggled", change_polling_rate, data);
    widget_add_event(builder, "pollingRate2", "toggled", change_polling_rate, data);
    widget_add_event(builder, "pollingRate3", "toggled", change_polling_rate, data);
}