#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "types.h"

#include "device/buttons.h"
#include "hid_keyboard_map.h"
#include "mouse_config.h"

#include "./templates/mouse_macro_button.h"
#include "./templates/macro_event_item.h"

static void claim_click(GtkGesture *gesture, int n_press, double x, double y, void *data);

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

/**
 * @brief Doubles the capacity of a one-dimensional array when necessary.
 * 
 * @param array The array to resize
 * @param type_size Size of the array element type
 * @param capacity A pointer to a int containing the current array capacity
 * @param element_count The number of elements in the array
 */
static void resize_array(void** array, size_t type_size, int *capacity, int element_count) {
    if (element_count == *capacity) {
        *capacity *= 2;
        *array = realloc(*array, type_size * (*capacity));
    }
}

/**
 * @brief Removes an element from the array.
 * 
 * @param array The array to remove the element from
 * @param type_size Size of the array element type
 * @param element_count The number of elements in the array
 * @param index The index pf the element to remove
 */
static void delete_array_element(void** array, size_t type_size, int element_count, int index) {
    if (index < 0 || index >= element_count) {
        printf("Index %d out of bounds for array length %d\n", index, element_count);
        exit(0);
        return;
    }

    byte **arr = (byte**) array;

    if (index < element_count - 1) {
        memmove(
            *arr + (index * type_size), 
            *arr + ((index + 1) * type_size),
            ((element_count - 1) - index) * type_size
        );
    }
}

/**
 * @brief Stores a MacroEventItem widget into a GListStore.
 * 
 * @param list_store_macro_events The GListStore to store the widget in
 * @param event A generic_macro_event object
 * @param data Application wide data structure
 */
static void list_store_add_macro_event(GListStore *list_store_macro_events, generic_macro_event *event, app_data *data) {
    if (event->delay > 0) {
        g_list_store_append(
            list_store_macro_events,
            macro_event_item_new(MACRO_ITEM_TYPE_DELAY, NULL, event->delay)
        );
    }

    const char* action_name = (event->event_type == MACRO_EVENT_TYPE_KEYBOARD)?
        data->button_data.key_names[event->action]:
        data->macro_data.mouse_button_names[event->action];
    
    g_list_store_append(
        list_store_macro_events,
        macro_event_item_new(MACRO_ITEM_TYPE_ACTION, action_name, event->delay)
    );
}

static void add_macro_event(MACRO_EVENT_TYPE event_type, byte action, MACRO_ACTION_TYPE action_type, app_data* data) {
    if (!data->macro_data.is_recording_macro) return;

    uint32_t macro_index = data->macro_data.macro_index;
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
        
        if (data->macro_data.is_resuming_macro_recording) {
            delay = 20;
            data->macro_data.is_resuming_macro_recording = false;
        }

        macro->events[previous_event_index].delay_next_action = delay;
    }
    
    resize_array((void**) &macro->events, sizeof(generic_macro_event), &macro->generic_event_array_size, macro->generic_event_count);
    
    event.delay = delay;
    event.delay_next_action = current_time;
    macro->events[macro->generic_event_count] = event;
    
    list_store_add_macro_event(data->macro_data.list_store_macro_events, &macro->events[macro->generic_event_count], data);
    macro->generic_event_count++;
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

static void append_macro_mouse_released(GtkGesture *self, int n_press, double x, double y, app_data *data) {
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_EVENT_TYPE_MOUSE, data->macro_data.mouse_buttons[gtk_button], MACRO_ACTION_TYPE_UP, data);
}

static void toggle_macro_recording(GtkGesture *gesture, int n_press, double x, double y, app_data *data) {
    claim_click(gesture, n_press, x, y, NULL);

    bool is_recording = !data->macro_data.is_recording_macro;
    bool is_resuming_recording = 
        is_recording &&
        !data->macro_data.is_resuming_macro_recording && 
        data->macro_data.macros[data->macro_data.macro_index].generic_event_count > 0;

    data->macro_data.is_recording_macro = is_recording;
    data->macro_data.is_resuming_macro_recording = is_resuming_recording;

    gtk_widget_set_visible(GTK_WIDGET(data->macro_data.image_recording_macro), data->macro_data.is_recording_macro);
}

static void create_macro(GSimpleAction *action, GVariant *variant, app_data *data) {
    resize_array((void **) &data->macro_data.macros, sizeof(mouse_macro), &data->macro_data.macro_array_size, data->macro_data.macro_count);

    mouse_macro *macro = &(data->macro_data.macros[data->macro_data.macro_count]);
    macro->generic_event_array_size = 2;    
    macro->events = malloc(sizeof(generic_macro_event) * macro->generic_event_array_size);
    macro->generic_event_count = 0;
    macro->name = NULL;

    data->macro_data.macro_index = data->macro_data.macro_count;
    
    gtk_editable_set_text(data->macro_data.editable_macro_name, "New Macro");
	gtk_overlay_add_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));

    menu_button_set_popover_visibility(get_active_menu_button(data), false);
}

static void stop_macro_recording(app_data *data) {
    data->macro_data.is_recording_macro = false;
    gtk_widget_set_visible(GTK_WIDGET(data->macro_data.image_recording_macro), false);

    g_list_store_remove_all(data->macro_data.list_store_macro_events);
	gtk_overlay_remove_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
    menu_button_set_popover_visibility(get_active_menu_button(data), true);
}

/**
 * @brief A function to close the macro overlay without saving the macro.
 * 
 * @param button Unused
 * @param data Application wide data structure
 */
static void close_macro_overlay(GtkButton *button, app_data *data) {
    stop_macro_recording(data);
    
    mouse_macro *macro = &(data->macro_data.macros[data->macro_data.macro_index]);

    if (data->macro_data.macro_index == data->macro_data.macro_count) {
        free(macro->events);
        free(macro->name);
    } else {
        // Effectively discards unsaved macro events
        macro->generic_event_count = data->macro_data.macro_saved_event_count;
    }
}

static void add_recorded_macro(char* macro_name, app_data *data) {
    gtk_list_box_append(
        data->macro_data.box_saved_macros, 
        GTK_WIDGET(mouse_macro_button_new(macro_name, data->macro_data.macro_count))
    );

    data->macro_data.macro_count++;
}

static void save_recorded_macro(GtkGesture *gesture, int n_press, double x, double y, app_data *data) {
    claim_click(gesture, n_press, x, y, NULL);
    stop_macro_recording(data);

    uint32_t macro_index = data->macro_data.macro_index;
    mouse_macro *macro = &data->macro_data.macros[macro_index];
    free(macro->name);

    char *macro_name = gtk_editable_get_chars(data->macro_data.editable_macro_name, 0, -1);
    macro->name = macro_name;

    if (data->macro_data.macro_index == data->macro_data.macro_count) {
        add_recorded_macro(macro_name, data);
    } else {
        GtkListBox *box_saved_macros = data->macro_data.box_saved_macros;
        MouseMacroButton *macro_button = MOUSE_MACRO_BUTTON(gtk_list_box_row_get_child(
            gtk_list_box_get_row_at_index(box_saved_macros, macro_index)
        ));

        gtk_widget_set_name(GTK_WIDGET(&macro_button->parent_instance), macro_name);
    }
}

static void edit_macro(GSimpleAction *action, GVariant *macro_index, app_data *data) {
    uint32_t index = g_variant_get_uint32(macro_index);

    mouse_macro macro = data->macro_data.macros[index];
    int event_count = macro.generic_event_count;

    for (int i = 0; i < event_count; i++) {
        generic_macro_event event = macro.events[i];
        list_store_add_macro_event(data->macro_data.list_store_macro_events, &event, data);
    }

    data->macro_data.macro_index = index;
    data->macro_data.macro_saved_event_count = event_count;

    gtk_editable_set_text(data->macro_data.editable_macro_name, macro.name);
    menu_button_set_popover_visibility(get_active_menu_button(data), false);
    gtk_overlay_add_overlay(data->widgets->overlay, GTK_WIDGET(data->widgets->box_macro));
}

static void update_macro_assignments(uint32_t macro_index, app_data *data) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        int button_macro_index = data->macro_data.macro_indicies[i];

        // All macros to the right of the deleted macro are shifted to the left,
        // so we update the macro indicies accordingly.
        if (button_macro_index > macro_index) {
            data->macro_data.macro_indicies[i]--;
        }

        if (button_macro_index == macro_index) {
            data->button_data.bindings[i] = data->button_data.default_bindings[i];
            data->macro_data.macro_indicies[i] = -1;

            g_mutex_lock(data->mouse->mutex);
            assign_button_action(data->mouse->dev, i, data->button_data.bindings[i]);
            g_mutex_unlock(data->mouse->mutex);

            GtkMenuButton *menu_button = data->button_data.menu_button_bindings[i];
            gtk_menu_button_set_label(
                menu_button,
                gtk_widget_get_tooltip_text(GTK_WIDGET(menu_button))
            );
        }
    }
}

static void delete_macro(GSimpleAction *action, GVariant *variant, app_data *data) {
    uint32_t macro_index = g_variant_get_uint32(variant);

    mouse_macro *macros = data->macro_data.macros;
    free(macros[macro_index].events);
    free(macros[macro_index].name);

    delete_array_element(
        (void**) &data->macro_data.macros,
        sizeof(mouse_macro),
        data->macro_data.macro_count,
        macro_index
    );
    
    data->macro_data.macro_count--;
    
    GtkListBox *box_saved_macros = data->macro_data.box_saved_macros;
    gtk_list_box_remove(
        box_saved_macros,
        GTK_WIDGET(gtk_list_box_get_row_at_index(box_saved_macros, macro_index))
    );

    for (int i = 0; i < data->macro_data.macro_count; i++) {
        MouseMacroButton *macro_button = MOUSE_MACRO_BUTTON(
            gtk_list_box_row_get_child(gtk_list_box_get_row_at_index(box_saved_macros, i))
        );

        gtk_actionable_set_action_target(GTK_ACTIONABLE(macro_button->button_name), (const char*) G_VARIANT_TYPE_UINT32, i);
        gtk_actionable_set_action_target(GTK_ACTIONABLE(macro_button->button_delete), (const char*) G_VARIANT_TYPE_UINT32, i);
    }

    update_macro_assignments(macro_index, data);
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
            memset(&keys_down, false, sizeof(keys_down));
            memset(&event_keys, false, sizeof(event_keys));
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

void assign_macro(uint32_t index, app_data *data) {
    // printf("%d\n", data->macro_data.macro_count);
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

    data->button_data.bindings[data->button_data.selected_button] = MOUSE_ACTION_TYPE_MACRO << 8;
    data->macro_data.macro_indicies[data->button_data.selected_button] = index;

    gtk_menu_button_set_label(get_active_menu_button(data), data->macro_data.macros[index].name);
}

static void select_macro(GSimpleAction *action, GVariant *macro_index, app_data *data) {
    uint32_t index = g_variant_get_uint32(macro_index);
    assign_macro(index, data);

    menu_button_set_popover_visibility(get_active_menu_button(data), false);
}

static void get_macro_data_widgets(GtkBuilder *builder, app_data *data) {
    data->widgets->box_macro = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxMacro")));
    data->widgets->macro_key_events = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "macroKeyController"));
    
    data->macro_data.gesture_macro_mouse_events = GTK_GESTURE(gtk_builder_get_object(builder, "gestureMacroMouseEvents"));
    data->macro_data.gesture_button_confirm_macro_claim_click = GTK_GESTURE(gtk_builder_get_object(builder, "gestureButtonConfirmMacro"));
    data->macro_data.gesture_button_record_macro_claim_click = GTK_GESTURE(gtk_builder_get_object(builder, "gestureButtonMacroRecording"));

    data->macro_data.flow_box_macro_events = GTK_FLOW_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "flowBoxMacroEvents")));
    data->macro_data.button_record_macro = GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "buttonMacroRecord")));
    data->macro_data.image_recording_macro = GTK_IMAGE(GTK_WIDGET(gtk_builder_get_object(builder, "imageRecordingMacro")));

    data->macro_data.button_confirm_macro = GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "buttonMacroSave")));
    
    data->macro_data.box_saved_macros = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxSavedMacros")));
    data->macro_data.editable_macro_name = GTK_EDITABLE(gtk_builder_get_object(builder, "editableMacroName"));
}

static void claim_click(GtkGesture *gesture, int n_press, double x, double y, void *data) {
    gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);
}

static void setup_event_controllers(app_data *data) {
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->widgets->macro_key_events);
    g_signal_connect(data->widgets->macro_key_events, "key-pressed", G_CALLBACK(append_macro_key_pressed), data);
    g_signal_connect(data->widgets->macro_key_events, "key-released", G_CALLBACK(append_macro_key_released), data);
    
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), GTK_EVENT_CONTROLLER(data->macro_data.gesture_macro_mouse_events));
    g_signal_connect(data->macro_data.gesture_macro_mouse_events, "pressed", G_CALLBACK(append_macro_mouse_pressed), data);
    g_signal_connect(data->macro_data.gesture_macro_mouse_events, "released", G_CALLBACK(append_macro_mouse_released), data);

    gtk_widget_add_controller(GTK_WIDGET(data->macro_data.button_record_macro), GTK_EVENT_CONTROLLER(data->macro_data.gesture_button_record_macro_claim_click));
    g_signal_connect(data->macro_data.gesture_button_record_macro_claim_click, "pressed", G_CALLBACK(toggle_macro_recording), data);
    
    gtk_widget_add_controller(GTK_WIDGET(data->macro_data.button_confirm_macro), GTK_EVENT_CONTROLLER(data->macro_data.gesture_button_confirm_macro_claim_click));
    g_signal_connect(data->macro_data.gesture_button_confirm_macro_claim_click, "pressed", G_CALLBACK(save_recorded_macro), data);
}

GtkWidget* create_macro_event_widget(MacroEventItem *item, void *data) {
    return GTK_WIDGET(item);
}

void destroy_macro_events(void *data) {

}

void app_config_macro_init(GtkBuilder *builder, app_data *data) {
    get_macro_data_widgets(builder, data);
    setup_event_controllers(data);

    const GActionEntry entries[] = {
        {.name = "add-macro",    .activate = (g_action) create_macro},
        {.name = "select-macro", .activate = (g_action) select_macro, .parameter_type = (const char*) G_VARIANT_TYPE_UINT32},
        {.name = "edit-macro",   .activate = (g_action) edit_macro,   .parameter_type = (const char*) G_VARIANT_TYPE_UINT32},
        {.name = "delete-macro", .activate = (g_action) delete_macro, .parameter_type = (const char*) G_VARIANT_TYPE_UINT32}
    };

    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
    
    mouse_macro *macros = data->macro_data.macros;

    for (int i = 0; i < data->macro_data.macro_count; i++) {
        gtk_list_box_append(
            data->macro_data.box_saved_macros, 
            GTK_WIDGET(mouse_macro_button_new(macros[i].name, i))
        );
    }

    data->macro_data.list_store_macro_events = g_list_store_new(G_TYPE_OBJECT);
    gtk_flow_box_bind_model(
        data->macro_data.flow_box_macro_events,
        (GListModel*) data->macro_data.list_store_macro_events,
        (GtkFlowBoxCreateWidgetFunc) create_macro_event_widget, 
        data,
        destroy_macro_events
    );

    widget_add_event(builder, "buttonMacroCancel", "clicked", close_macro_overlay, data);
}