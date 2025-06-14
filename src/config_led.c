#include <gtk/gtk.h>
#include <gtk/gtkapplication.h>

#include "types.h"
#include "device/rgb.h"
#include "config_led.h"

void app_config_led_init(GtkBuilder *builder, app_data *data) {
    data->widgets->color_chooser = GTK_COLOR_CHOOSER(GTK_WIDGET(gtk_builder_get_object(builder, "colorChooserLed")));
    data->widgets->range_brightness = GTK_RANGE(GTK_WIDGET(gtk_builder_get_object(builder, "scaleBrightness")));

    data->color_data = (config_color_data) {.mouse_led = data->mouse->led, .color_chooser = data->widgets->color_chooser};

    g_timeout_add(10, G_SOURCE_FUNC(update_color), &data->color_data);
    g_signal_connect(data->widgets->range_brightness, "value-changed", G_CALLBACK(update_brightness), &(data->mouse->led->brightness));
}

int update_color(config_color_data *color_data) {
	color_options *mouse_led = color_data->mouse_led;
    
	GdkRGBA color = {};
	gtk_color_chooser_get_rgba(color_data->color_chooser, &color);

	mouse_led->red = (byte) (color.red * 255);
	mouse_led->green = (byte) (color.green * 255);
	mouse_led->blue = (byte) (color.blue * 255);

	return G_SOURCE_CONTINUE;
}

void update_brightness(GtkRange *range_brightness, byte *mouse_led_brightness) {
    *mouse_led_brightness = gtk_range_get_value(range_brightness);
}