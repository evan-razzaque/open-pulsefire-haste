#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

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

static void resize_array(void** array, size_t type_size, int *capacity, int element_count) {
    if (element_count == *capacity) {
        *capacity *= 2;
        *array = realloc(*array, type_size * (*capacity));
    }
}

static void add_macro_event(MACRO_EVENT_TYPE event_type, byte action, MACRO_ACTION_TYPE action_type, app_data* data) {
    if (!data->macro_data.recording_macro) return;

    int macro_index = data->macro_data.macro_count;
    mouse_macro *macro = &(data->macro_data.macros[macro_index]);
    int previous_event_index = macro->generic_event_count - 1;

    generic_macro_event event = {.event_type = event_type, .action_type = action_type, .action = action};
    
    // Prevents identical macro actions from being added multiple times in a row
    if (previous_event_index >= 0 && macro_event_equals(macro->events[previous_event_index], event)) {
        return;
    }

    struct timeval t;
    gettimeofday(&t, NULL);

    long current_time = (t.tv_sec * 1000) + (t.tv_usec / 1000);
    int delay = 0;

    if (previous_event_index >= 0) {
        delay = current_time - (long) macro->events[previous_event_index].delay_next_action;
        delay = MIN(delay, 9999);
        macro->events[previous_event_index].delay_next_action = delay;
    }

    resize_array((void**) &macro->events, sizeof(generic_macro_event), &macro->generic_event_array_size, macro->generic_event_count);

    event.delay_next_action = current_time;
    macro->events[macro->generic_event_count++] = event;

    const char* old_macro_string = gtk_label_get_text(data->widgets->label_macro_events);
    char *macro_string = malloc(sizeof(char) * (strlen(old_macro_string) + 40));

    sprintf(macro_string, "%s  %d  <%s - %d>", old_macro_string, delay, (event_type == MACRO_EVENT_TYPE_KEYBOARD)? data->button_data.key_names[action] : data->macro_data.mouse_button_names[action], action_type);
    gtk_label_set_text(data->widgets->label_macro_events, macro_string);
    free(macro_string);
}

static void append_macro_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
    add_macro_event(MACRO_EVENT_TYPE_KEYBOARD, data->button_data.keyboard_keys[keyval], MACRO_ACTION_TYPE_DOWN, data);
}

static void append_macro_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
    add_macro_event(MACRO_EVENT_TYPE_KEYBOARD, data->button_data.keyboard_keys[keyval], MACRO_ACTION_TYPE_UP, data);
}

static void append_macro_mouse_pressed(GtkGesture* self, int n_press, double x, double y, app_data *data) {
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_EVENT_TYPE_MOUSE, data->macro_data.mouse_buttons[gtk_button], MACRO_ACTION_TYPE_DOWN, data);
}

static void append_macro_mouse_released(GtkGesture* self, int n_press, double x, double y, app_data *data) {
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_EVENT_TYPE_MOUSE, data->macro_data.mouse_buttons[gtk_button], MACRO_ACTION_TYPE_UP, data);
}

static void toggle_macro_recording(GtkButton* self, app_data *data) {
    bool is_recording = data->macro_data.recording_macro;
    data->macro_data.recording_macro = !is_recording;
}

static void open_macro_overlay(GSimpleAction *action, GVariant *variant, app_data *data) {
    resize_array((void **) &data->macro_data.macros, sizeof(mouse_macro), &data->macro_data.macro_array_size, data->macro_data.macro_count);
    
    mouse_macro *macro = &(data->macro_data.macros[data->macro_data.macro_count]);
    macro->generic_event_array_size = 2;
    macro->events = malloc(sizeof(generic_macro_event) * macro->generic_event_array_size);
    macro->generic_event_count = 0;
    macro->macro_name = NULL;
    
    gtk_editable_set_text(data->macro_data.editable_macro_name, "New Macro");
	gtk_overlay_add_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

static void close_macro_overlay(GtkButton *self, app_data *data) {
    data->macro_data.recording_macro = false;
    
    mouse_macro *macro = &(data->macro_data.macros[data->macro_data.macro_count]);
    free(macro->events);
    free(macro->macro_name);

    gtk_label_set_text(data->widgets->label_macro_events, "");
	gtk_overlay_remove_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

static void save_recorded_macro(GtkGesture* self, int n_press, double x, double y, app_data *data) {
    // Prevents the button click from being included in the macro.
    gtk_gesture_set_state(data->macro_data.gesture_button_confirm_macro, GTK_EVENT_SEQUENCE_CLAIMED);

    data->macro_data.recording_macro = false;
    
    char *macro_name = gtk_editable_get_chars(data->macro_data.editable_macro_name, 0, -1);
    data->macro_data.macros[data->macro_data.macro_count].macro_name = macro_name;

    GMenuItem *macro_item = g_menu_item_new(macro_name, NULL);
    g_menu_item_set_action_and_target(macro_item, "app.select-macro", (const char*) G_VARIANT_TYPE_UINT32, data->macro_data.macro_count);
    g_menu_append_item(data->macro_data.menu_macros, macro_item);

    data->macro_data.macro_count++;

    gtk_label_set_text(data->widgets->label_macro_events, "");
	gtk_overlay_remove_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

/**
 * @brief Converts a mouse macro into an array of macro events
 * that are readable by the mouse.
 * 
 * @param macro The recored macro
 * @param events The output location to store macro events into
 * @param modifier_map Used to map modifier keys to modifier bit flags for a macro key event
 * @return the number of macro events
 */
static int parse_macro(mouse_macro macro, macro_event *events, byte *modifier_map) {
    int event_count = 0, event_index = 0;

    bool keys_down[256] = {false}, event_keys[256] = {false};
    int keys_down_count = 0, event_keys_count = 0;
    
    bool is_mouse_down = false;

    for (int i = 0; i < macro.generic_event_count; i++) {
        generic_macro_event event = macro.events[i];
        uint16_t event_action_type = (uint16_t) (event.event_type << 8) + (uint16_t) event.action_type; 

        switch (event_action_type) {
        case KEY_DOWN:
            if (is_mouse_down || event_keys[event.action]) break;
            
            if (keys_down_count == 0) {
                events[event_index] = KEYBOARD_EVENT_DOWN(0, event.delay_next_action, 0);
                event_count++;
            } else if (event_keys_count < 6) {
                int previous_delay = events[event_index].key_event.delay_next_action;
                events[event_index].key_event.delay_next_action = MAX(previous_delay, event.delay_next_action);
            } else {
                break;
            }

            event_keys[event.action] = true;
            keys_down[event.action] = true;
            keys_down_count++;

            if (event.action >= 0xe0) {
                events[event_index].key_event.modifier_keys += modifier_map[event.action];
                break;
            }

            events[event_index].key_event.keys[event_keys_count] = event.action;
            event_keys_count++;
            break;
        case KEY_UP:
            if (is_mouse_down) break;
            if (!event_keys[event.action] || !keys_down[event.action]) break;

            keys_down[event.action] = false;
            keys_down_count--;
            
            if (keys_down_count > 0) break;
            event_count++;

            event_index++;
            events[event_index] = KEYBOARD_EVENT_UP(event.delay_next_action);
            event_index++;
            
            event_keys_count = 0;
            memset(&keys_down, false, 256);
            memset(&event_keys, false, 256);
            break;
        case MOUSE_DOWN:
            if (keys_down_count > 0) break;

            event_count += 2;
            is_mouse_down = true;
            events[event_index] = MOUSE_EVENT(event.action, event.delay_next_action, 0);

            break;
        case MOUSE_UP: 
            if (keys_down_count > 0) break;

            is_mouse_down = false;
            events[event_index].mouse_event.up.delay_next_action = event.delay_next_action;
            event_index++;
            
            break;
        default:
            break;
        }
    }

    if (is_mouse_down || keys_down_count > 0) return -1;
    return event_count;
}

static void select_macro(GSimpleAction *action, GVariant *macro_index, app_data *data) {
    uint32_t index = g_variant_get_uint32(macro_index);
    mouse_macro macro = data->macro_data.macros[index];

    macro_event *events = malloc(sizeof(macro_event) * macro.generic_event_count);
    int event_count = parse_macro(macro, events, data->macro_data.modifier_map);

    if (event_count < 0) {
        return;
    }

    g_mutex_lock(data->mouse->mutex);
    assign_button_macro(
        data->mouse->dev,
        data->button_data.selected_button, 
        MACRO_REPEAT_MODE_ONCE,
        events,
        event_count
    );
    g_mutex_unlock(data->mouse->mutex);

    free(events);

    GtkMenuButton *menu_button_active = data->button_data.menu_button_bindings[data->button_data.selected_button];
    gtk_menu_button_set_label(menu_button_active, data->macro_data.macros[index].macro_name);
}

void get_macro_data_widgets(GtkBuilder *builder, app_data *data) {
    data->widgets->box_macro = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMacro")));
    data->widgets->macro_key_events = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "macroKeyController"));
    data->widgets->label_macro_events = GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(builder, "labelMacroEvents")));

    data->macro_data.gesture_macro_mouse_events = GTK_GESTURE(gtk_builder_get_object(builder, "gestureMacroMouseEvents"));
    data->macro_data.gesture_button_confirm_macro = GTK_GESTURE(gtk_builder_get_object(builder, "gestureButtonConfirmMacro"));
    data->macro_data.button_confirm_macro = GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "buttonMacroSave")));

    data->macro_data.menu_macros = G_MENU(gtk_builder_get_object(builder, "menuMacros"));
    data->macro_data.editable_macro_name = GTK_EDITABLE(gtk_builder_get_object(builder, "editableMacroName"));
}

static void setup_event_controllers(app_data *data) {
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->widgets->macro_key_events);
    g_signal_connect(data->widgets->macro_key_events, "key-pressed", G_CALLBACK(append_macro_key_pressed), data);
    g_signal_connect(data->widgets->macro_key_events, "key-released", G_CALLBACK(append_macro_key_released), data);
    
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), GTK_EVENT_CONTROLLER(data->macro_data.gesture_macro_mouse_events));
    g_signal_connect(data->macro_data.gesture_macro_mouse_events, "pressed", G_CALLBACK(append_macro_mouse_pressed), data);
    g_signal_connect(data->macro_data.gesture_macro_mouse_events, "released", G_CALLBACK(append_macro_mouse_released), data);

    gtk_widget_add_controller(GTK_WIDGET(data->macro_data.button_confirm_macro), GTK_EVENT_CONTROLLER(data->macro_data.gesture_button_confirm_macro));
    g_signal_connect(data->macro_data.gesture_button_confirm_macro, "pressed", G_CALLBACK(save_recorded_macro), data);
}

void app_config_macro_init(GtkBuilder *builder, app_data *data) {
    get_macro_data_widgets(builder, data);
    setup_event_controllers(data);

    const GActionEntry entries[] = {
        {.name = "add-macro", .activate = (g_action) open_macro_overlay},
        {.name = "select-macro", .activate = (g_action) select_macro, .parameter_type = (const char*) G_VARIANT_TYPE_UINT32}
    };

	g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);

    widget_add_event(builder, "buttonRecord", "clicked", toggle_macro_recording, data);
    widget_add_event(builder, "buttonMacroCancel", "clicked", close_macro_overlay, data);
}