#include <gtk/gtk.h>

#include "types.h"
#include "device/buttons.h"
#include "device/rgb.h"
#include "config_buttons.h"

void app_config_buttons_init(GtkBuilder *builder, app_data *data) {
    data->widgets->event_key_controller = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "eventKeyController"));
    data->widgets->label_pressed_key = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelPressedKey")));
	gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->widgets->event_key_controller);

	GtkDropDown *dropdown_mouse_forward = GTK_DROP_DOWN(GTK_WIDGET(gtk_builder_get_object(builder, "dropdownMouseForward")));
	
    g_signal_connect(data->widgets->event_key_controller, "key-pressed", G_CALLBACK(key_pressed_display), data->widgets->label_pressed_key);
	g_signal_connect(dropdown_mouse_forward, "notify::selected-item", G_CALLBACK(print_selected_item), data);
}

void add_action_values(mouse_action_values *action_values) {
	action_values->mouse[0] = LEFT_CLICK;
	action_values->mouse[1] = RIGHT_CLICK;
	action_values->mouse[2] = MIDDLE_CLICK;
	action_values->mouse[3] = BACK;
	action_values->mouse[4] = FORWARD;
	action_values->mouse[5] = DPI_SWITCH;

	action_values->media[0] = PLAY_PAUSE;
	action_values->media[1] = STOP;
	action_values->media[2] = PREVIOUS;
	action_values->media[3] = NEXT;
	action_values->media[4] = MUTE;
	action_values->media[5] = VOLUME_DOWN;
	action_values->media[6] = VOLUME_UP;
}

int key_pressed_display(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, GtkWidget* data) {
	GtkLabel *label_pressed_key = GTK_LABEL(data);

	char key_press_info[64];

	sprintf(key_press_info, "Value: %u, Code: %u, Modfifier: %d", keyval, keycode, state);
	gtk_label_set_text(label_pressed_key, key_press_info);

	return TRUE;
}

void print_selected_item(GtkDropDown *self, GParamSpec *param_spec, void *data) { 
	mouse_data *mouse  = ((app_data*) data)->mouse;
	mouse_action_values *action_values = ((app_data*) data)->action_values;

	int action_index = gtk_drop_down_get_selected(self);
	int action_value = action_values->mouse[action_index];
	printf("%.4x\n", action_value);

	assign_button(mouse->dev, MOUSE_BUTTON_SIDE_FORWARD, action_value);
}