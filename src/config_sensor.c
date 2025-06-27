#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "device/mouse.h"

#include "types.h"
#include "mouse_config.h"

static void change_polling_rate(GSimpleAction* action, GVariant *value, app_data *data) {
    g_simple_action_set_state(action, value);
    printf("%d\n", g_variant_get_int32(value));
    set_polling_rate(data->mouse->dev, g_variant_get_int32(value));
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    GSimpleAction *action_change_polling_rate = g_simple_action_new_stateful("change-polling-rate", G_VARIANT_TYPE_INT32, g_variant_new_int32(3));
    g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_change_polling_rate));
    g_signal_connect(action_change_polling_rate, "change-state", G_CALLBACK(change_polling_rate), data);
}