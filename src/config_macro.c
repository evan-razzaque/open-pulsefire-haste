#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdint.h>
#include <sys/time.h>

#include "types.h"

#include "device/buttons.h"
#include "hid_keyboard_map.h"
#include "mouse_config.h"

/**
 * @brief Determines if two generic_macro_events are equal.
 * 
 * @param x The first macro event
 * @param y The second macro event
 * @return whether the macro events are equal or not
 */
static bool macro_event_equals(generic_macro_event x, generic_macro_event y) {
    return x.event_type == y.event_type && x.action_type == y.action_type && x.action == y.action;
}

static void add_macro_event(MACRO_EVENT_TYPE event_type, byte action, MACRO_ACTION_TYPE action_type, app_data* data) {
    if (!data->macro_data.recording_macro) return;

    config_macro_data *macro_data = &data->macro_data;
    int previous_event_index = macro_data->event_index - 1;

    generic_macro_event event = {.event_type = event_type, .action_type = action_type, .action = action};
    
    if (previous_event_index >= 0 && macro_event_equals(macro_data->events[previous_event_index], event)) {
        return;
    }

    struct timeval t;
    gettimeofday(&t, NULL);

    long current_time = (t.tv_sec * 1000) + (t.tv_usec / 1000);
    int delay = 0;

    if (previous_event_index >= 0) {
        delay = current_time - (long) macro_data->events[previous_event_index].delay_next_action;
        delay = MIN(delay, 9999);
        macro_data->events[previous_event_index].delay_next_action = delay;
    }

    if (macro_data->event_array_size == macro_data->event_index) {
        macro_data->event_array_size *= 2;
        macro_data->events = realloc(macro_data->events, sizeof(generic_macro_event) * macro_data->event_array_size);
    }

    event.delay_next_action = current_time;
    macro_data->events[macro_data->event_index++] = event;

    const char* old_macro_string = gtk_label_get_text(data->widgets->label_macro_events);
    char macro_string[strlen(old_macro_string) + 40];

    sprintf(macro_string, "%s  %d  <%s - %d>", old_macro_string, delay, (event_type == MACRO_EVENT_TYPE_KEYBOARD)? data->button_data.key_names[action] : macro_data->mouse_button_names[action], action_type);
    gtk_label_set_text(data->widgets->label_macro_events, macro_string);
}

static void append_macro_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
    add_macro_event(MACRO_EVENT_TYPE_KEYBOARD, data->button_data.keyboard_keys[keyval], MACRO_ACTION_TYPE_DOWN, data);
}

static void append_macro_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
    add_macro_event(MACRO_EVENT_TYPE_KEYBOARD, data->button_data.keyboard_keys[keyval], MACRO_ACTION_TYPE_UP, data);
}

static void append_macro_mouse_pressed(GtkGesture* self, int n_press, double x, double y, app_data *data) {
    printf("Mouse pressed\n");
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_EVENT_TYPE_MOUSE, data->macro_data.mouse_buttons[gtk_button], MACRO_ACTION_TYPE_DOWN, data);
}

static void append_macro_mouse_released(GtkGesture* self, int n_press, double x, double y, app_data *data) {
    printf("Mouse released\n");
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_EVENT_TYPE_MOUSE, data->macro_data.mouse_buttons[gtk_button], MACRO_ACTION_TYPE_UP, data);
}

static void open_macro_overlay(GSimpleAction *action, GVariant *variant, app_data *data) {
    data->macro_data.recording_macro = TRUE;
    data->macro_data.event_array_size = 2;
    data->macro_data.event_index = 0;
    data->macro_data.events = malloc(sizeof(generic_macro_event) * data->macro_data.event_array_size);

	gtk_overlay_add_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

static void close_macro_overlay(GtkButton *self, app_data *data) {
    data->macro_data.recording_macro = FALSE;
    free(data->macro_data.events);

    gtk_label_set_text(data->widgets->label_macro_events, "");
	gtk_overlay_remove_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

static void save_recorded_macro(GtkButton *self, app_data *data) {
    printf("Macro Saved\n");

    data->macro_data.recording_macro = FALSE;
    free(data->macro_data.events);

    gtk_label_set_text(data->widgets->label_macro_events, "");
	gtk_overlay_remove_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

void app_config_macro_init(GtkBuilder *builder, app_data *data) {
    // macro_event events[1] = {
	// 	MOUSE_EVENT(0x10, 50, 50)
	// };

	// g_mutex_lock(data->mouse->mutex);
	// assign_button_macro(data->mouse->dev, MACRO_BINDING_FORWARD, MACRO_REPEAT_MODE_ONCE, events, 2);
	// g_mutex_unlock(data->mouse->mutex);

    data->widgets->box_macro = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMacro")));
    data->widgets->macro_key_events = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "macroKeyController"));
    data->widgets->label_macro_events = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelMacroEvents")));

    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->widgets->macro_key_events);
    g_signal_connect(data->widgets->macro_key_events, "key-pressed", G_CALLBACK(append_macro_key_pressed), data);
    g_signal_connect(data->widgets->macro_key_events, "key-released", G_CALLBACK(append_macro_key_released), data);

    GtkGesture *gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 0);
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), GTK_EVENT_CONTROLLER(gesture));

    g_signal_connect(gesture, "pressed", G_CALLBACK(append_macro_mouse_pressed), data);
    g_signal_connect(gesture, "released", G_CALLBACK(append_macro_mouse_released), data);

    GSimpleAction *action_add_macro = g_simple_action_new("add-macro", NULL);
	g_action_map_add_action(G_ACTION_MAP(data->widgets->app), G_ACTION(action_add_macro));	
	g_signal_connect(action_add_macro, "activate", G_CALLBACK(open_macro_overlay), data);

    widget_add_event(builder, "buttonMacroCancel", "clicked", close_macro_overlay, data);
    widget_add_event(builder, "buttonMacroSave", "clicked", save_recorded_macro, data);

}