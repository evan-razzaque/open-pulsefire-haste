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

static void add_dpi_level(GSimpleAction* action, GVariant *value, app_data *data) {
    printf("Dpi level\n");
}

void app_config_sensor_init(GtkBuilder *builder, app_data *data) {
    const GActionEntry entries[] = {
        {.name = "change-polling-rate", .change_state = (g_action) change_polling_rate, .parameter_type = (const char*) G_VARIANT_TYPE_INT32, .state = "3"},
        {.name = "add-dpi-level", .activate = (g_action) add_dpi_level}
    };
    
    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
}