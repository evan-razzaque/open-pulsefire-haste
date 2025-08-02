#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "device/buttons.h"

#include "types.h"
#include "util.h"
#include "mouse_config.h"
#include "config_macro.h"
#include "config_buttons.h"

#define MACRO_PARSER_PRIVATE
#include "macro_parser.h"

#include "templates/mouse_macro_button.h"
#include "templates/macro_event_item.h"

#define MAX_MACRO_EVENT_COUNT (80)

static void claim_click(GtkGesture *gesture, void *data);

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
 * @brief Stores a MacroEventItem widget into a wrap box.
 * 
 * @param wrap_box The wrap box
 * @param event The macro event for the MacroEventItem widget
 * @param data Application wide data structure
 */
static void wrap_box_add_macro_event(AdwWrapBox *wrap_box, generic_macro_event *event, app_data *data) {
    const char* action_name = (event->action_type == MACRO_ACTION_TYPE_KEYBOARD)?
        data->button_data->key_names[event->action]:
        data->macro_data->mouse_button_names[event->action];

    adw_wrap_box_append(
        wrap_box, 
        GTK_WIDGET(
            macro_event_item_new(action_name, event->delay, event->event_type)
        )
    );    
}

/**
 * @brief Adds a macro event to the macro being recoreded.
 * 
 * @param action_type A MACRO_ACTION_TYPE value
 * @param action An action value given the action_type
 * @param event_type A MACRO_EVENT_TYPE value
 * @param data Application wide data structure
 */
static void add_macro_event(MACRO_ACTION_TYPE action_type, byte action, MACRO_EVENT_TYPE event_type, app_data* data) {
    if (!data->macro_data->is_recording_macro) return;

    uint32_t macro_index = data->macro_data->macro_index;
    recorded_macro *macro = &(data->macro_data->macros[macro_index]);

    if (macro->generic_event_count >= MAX_MACRO_EVENT_COUNT) return;

    int previous_event_index = macro->generic_event_count - 1;

    generic_macro_event event = {.action_type = action_type, .event_type = event_type, .action = action};

    struct timeval t;
    gettimeofday(&t, NULL);

    long current_time = (t.tv_sec * 1000) + (t.tv_usec / 1000);
    int delay = 0;

    if (previous_event_index >= 0) {
        delay = current_time - (long) macro->events[previous_event_index].delay_next_event;
        delay = MIN(delay, 9999);
        
        if (data->macro_data->is_resuming_macro_recording) {
            delay = 20;
            data->macro_data->is_resuming_macro_recording = false;
        }

        macro->events[previous_event_index].delay_next_event = delay;
    }
    
    resize_array((void**) &macro->events, sizeof(generic_macro_event), &macro->generic_event_array_size, macro->generic_event_count);
    
    event.delay = delay;
    event.delay_next_event = current_time;
    macro->events[macro->generic_event_count] = event;
    
    wrap_box_add_macro_event(
        data->macro_data->wrap_box_macro_events,
        macro->events + macro->generic_event_count,
        data
    );
    macro->generic_event_count++;
}

/**
 * @brief Appends a key press to the macro being recoreded.
 * 
 * @param self The GtkEventControllerKey instance
 * @param keyval The value of the key pressed
 * @param keycode Unused
 * @param state Unused
 * @param data Application wide data structure
 */
static void append_macro_key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
    if (data->macro_data->last_pressed_key == keyval) return;
    data->macro_data->last_pressed_key = keyval;

    add_macro_event(MACRO_ACTION_TYPE_KEYBOARD, data->button_data->keyboard_keys[keyval], MACRO_EVENT_TYPE_DOWN, data);
}

/**
 * @brief Appends a key release to the macro being recoreded.
 * 
 * @param self The GtkEventControllerKey instance
 * @param keyval The value of the key pressed
 * @param keycode Unused
 * @param state Unused
 * @param data Application wide data structure
 */
static void append_macro_key_released(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, app_data* data) {
    data->macro_data->last_pressed_key = 0;

    add_macro_event(MACRO_ACTION_TYPE_KEYBOARD, data->button_data->keyboard_keys[keyval], MACRO_EVENT_TYPE_UP, data);
}

/**
 * @brief Appends a mouse button press to the macro being recorded.
 * 
 * @param self The GtkGesture instance
 * @param n_press Unused
 * @param x Unused
 * @param y Unused
 * @param data Application wide data structure
 */
static void append_macro_mouse_pressed(GtkGesture* self, int n_press, double x, double y, app_data *data) {
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_ACTION_TYPE_MOUSE, data->macro_data->mouse_buttons[gtk_button], MACRO_EVENT_TYPE_DOWN, data);
}

/**
 * @brief Appends a mouse button release to the macro being recorded.
 * 
 * @param self The GtkGesture instance
 * @param n_press Unused
 * @param x Unused
 * @param y Unused
 * @param data Application wide data structure
 */
static void append_macro_mouse_released(GtkGesture *self, int n_press, double x, double y, app_data *data) {
    uint32_t gtk_button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
    add_macro_event(MACRO_ACTION_TYPE_MOUSE, data->macro_data->mouse_buttons[gtk_button], MACRO_EVENT_TYPE_UP, data);
}

/**
 * @brief Starts/stops the macro recording
 * 
 * @param gesture The GtkGesture instance used in claim_click()
 * @param n_press Unused
 * @param x Unused
 * @param y Unused
 * @param data Application wide data structure
 */
static void toggle_macro_recording(GtkGesture *gesture, int n_press, double x, double y, app_data *data) {
    claim_click(gesture, NULL);

    bool is_recording = !data->macro_data->is_recording_macro;
    bool is_resuming_recording = 
        is_recording &&
        !data->macro_data->is_resuming_macro_recording && 
        data->macro_data->macros[data->macro_data->macro_index].generic_event_count > 0;

    data->macro_data->is_recording_macro = is_recording;
    data->macro_data->is_resuming_macro_recording = is_resuming_recording;

    gtk_editable_label_stop_editing(
        (GtkEditableLabel*) data->macro_data->editable_macro_name,
        true
    );

    gtk_widget_set_visible(GTK_WIDGET(data->macro_data->image_recording_macro), is_recording);
    gtk_widget_set_sensitive(GTK_WIDGET(data->macro_data->editable_macro_name), !is_recording);
}

/**
 * @brief A function to create a new macro.
 * 
 * @param action Unused
 * @param variant Unused
 * @param data Application wide data structure
 */
static void create_macro(GSimpleAction *action, GVariant *variant, app_data *data) {
    resize_array((void **) &data->macro_data->macros, sizeof(recorded_macro), &data->macro_data->macro_array_size, data->macro_data->macro_count);

    recorded_macro *macro = &(data->macro_data->macros[data->macro_data->macro_count]);
    macro->generic_event_array_size = 2;    
    macro->events = malloc(sizeof(generic_macro_event) * macro->generic_event_array_size);
    macro->generic_event_count = 0;
    
    macro->repeat_mode = REPEAT_MODE_PLAY_ONCE;
    macro->name = NULL;

    data->macro_data->macro_index = data->macro_data->macro_count;
    
    gtk_editable_set_text(data->macro_data->editable_macro_name, "New Macro");
    gtk_drop_down_set_selected(data->macro_data->drop_down_repeat_mode, 0);
    menu_button_set_popover_visibility(get_active_menu_button(data->button_data), false);
}

/**
 * @brief A function to stop the macro recording and exit the macro stack page.
 * 
 * @param data Application wide date structure
 */
static void stop_macro_recording(app_data *data) {
    data->macro_data->is_recording_macro = false;
    gtk_widget_set_visible(GTK_WIDGET(data->macro_data->image_recording_macro), false);

    recorded_macro *macro = data->macro_data->macros + data->macro_data->macro_index;
    int last_event_index = macro->generic_event_count - 1;

    adw_wrap_box_remove_all(data->macro_data->wrap_box_macro_events);
    menu_button_set_popover_visibility(get_active_menu_button(data->button_data), true);
    
    if (last_event_index < 1) return;

    macro->events[last_event_index].delay_next_event = macro->events[last_event_index - 1].delay_next_event;
}

/**
 * @brief A function to close the macro overlay without saving the macro.
 * 
 * @param button Unused
 * @param data Application wide data structure
 */
static void close_macro_overlay(GtkButton *button, app_data *data) {
    stop_macro_recording(data);
    
    recorded_macro *macro = &(data->macro_data->macros[data->macro_data->macro_index]);

    if (data->macro_data->macro_index == data->macro_data->macro_count) {
        free(macro->events);
        free(macro->name);
    } else {
        macro->generic_event_count = data->macro_data->macro_previous_event_count;
        macro->repeat_mode = data->macro_data->macro_previous_repeat_mode;
    }
}

/**
 * @brief Creates a mouse macro button widget with the signals 
 * used to assign the macro.
 * 
 * @param macro_name The name of the macro
 * @param index The index of the macro
 * @param data Application wide data structure
 * @return MouseMacroButton widget
 */
static MouseMacroButton* create_macro_item(char *macro_name, byte index, app_data *data) {
    MouseMacroButton *self = mouse_macro_button_new(
        macro_name,
        index
    );

    gtk_list_box_append(
        data->macro_data->box_saved_macros,
        GTK_WIDGET(self)
    );

    return self;
}

/**
 * @brief A function edit a macro and re-assigned to buttons that are binded to the macro.
 * 
 * @param macro_index The index of the macro
 * @param macro_name The name of the macro
 * @param data Application wide data structure
 */
static void modify_recorded_macro(uint32_t macro_index, char *macro_name, app_data *data) {
    GtkListBox *box_saved_macros = data->macro_data->box_saved_macros;
    MouseMacroButton *macro_button = MOUSE_MACRO_BUTTON(gtk_list_box_row_get_child(
        gtk_list_box_get_row_at_index(box_saved_macros, macro_index)
    ));

    gtk_widget_set_name(GTK_WIDGET(macro_button), macro_name);

    int *macro_indicies = data->macro_data->macro_indicies;

    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (
            data->button_data->bindings[i] >> 8 == MOUSE_ACTION_TYPE_MACRO
            && macro_indicies[i] == macro_index
        ) {
            assign_macro(macro_indicies[i], i, data);
        }
    }
}

/**
 * @brief Saves changes to a macro.
 * 
 * @param gesture The GtkGesture instance used in claim_click()
 * @param n_press Unused
 * @param x Unused
 * @param y Unused
 * @param data Application wide data structure
 */
static void save_recorded_macro(GtkGesture *gesture, int n_press, double x, double y, app_data *data) {
    claim_click(gesture, NULL);
    stop_macro_recording(data);

    uint32_t macro_index = data->macro_data->macro_index;
    recorded_macro *macro = data->macro_data->macros + macro_index;
    free(macro->name);

    char *macro_name = gtk_editable_get_chars(data->macro_data->editable_macro_name, 0, -1);
    macro->name = macro_name;

    // New macro
    if (data->macro_data->macro_index == data->macro_data->macro_count) {
        create_macro_item(macro_name, data->macro_data->macro_count, data);
        data->macro_data->macro_count++;
    } else {
        modify_recorded_macro(macro_index, macro_name, data);
    }
}

/**
 * @brief Changes the macro's repeat mode behavior.
 * 
 * @param dropdown The dropdown widget containing the repeat mode options
 * @param param_spec Unused
 * @param data Application wide data structure
 */
static void change_macro_repeat_mode(GtkDropDown *dropdown, GParamSpec *param_spec, app_data *data) {
    uint32_t macro_index = data->macro_data->macro_index;
    recorded_macro *macro = data->macro_data->macros + macro_index;

    byte selected_index = gtk_drop_down_get_selected(dropdown);
    macro->repeat_mode = data->macro_data->repeat_mode_map[selected_index];
}

/**
 * @brief A function to open the stack page with the macro to be edited.
 * 
 * @param action Unused
 * @param macro_index The index of the macro
 * @param data Application wide data structure
 */
static void edit_macro(GSimpleAction *action, GVariant *macro_index, app_data *data) {
    gtk_widget_set_sensitive(GTK_WIDGET(data->widgets->box_main), false);
    gtk_stack_set_page(data->widgets->stack_main, STACK_PAGE_MACRO);

    uint32_t index = g_variant_get_uint32(macro_index);
    
    recorded_macro *macro = data->macro_data->macros + index;
    generic_macro_event *macro_events = macro->events;
    int event_count = macro->generic_event_count;

    for (int i = 0; i < event_count; i++) {
        wrap_box_add_macro_event(data->macro_data->wrap_box_macro_events, macro_events + i, data);
    }

    data->macro_data->macro_index = index;
    data->macro_data->macro_previous_event_count = event_count;
    data->macro_data->macro_previous_repeat_mode = macro->repeat_mode;

    gtk_editable_set_text(data->macro_data->editable_macro_name, macro->name);
    gtk_drop_down_set_selected(
        data->macro_data->drop_down_repeat_mode,
        array_index_of(data->macro_data->repeat_mode_map, REPEAT_MODES_COUNT, macro->repeat_mode)
    );

    menu_button_set_popover_visibility(get_active_menu_button(data->button_data), false);
}

/**
 * @brief A method to update macro assignments for each button 
 * when a macro is deleted.
 * 
 * @param macro_index The index of the deleted macro
 * @param data Application wide data structure
 */
static void update_macro_assignments(uint32_t macro_index, app_data *data) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        int button_macro_index = data->macro_data->macro_indicies[i];

        // All macros to the right of the deleted macro are shifted to the left,
        // so we update the macro indicies accordingly.
        if (button_macro_index > macro_index) {
            data->macro_data->macro_indicies[i]--;
        }

        if (button_macro_index != macro_index) continue;

        data->button_data->bindings[i] = data->button_data->default_bindings[i];
        data->macro_data->macro_indicies[i] = -1;

        g_mutex_lock(data->mouse->mutex);
        assign_button_action(data->mouse->dev, i, data->button_data->bindings[i]);
        g_mutex_unlock(data->mouse->mutex);

        GtkMenuButton *menu_button = data->button_data->menu_button_bindings[i];
        gtk_menu_button_set_label(
            menu_button,
            gtk_widget_get_tooltip_text(GTK_WIDGET(menu_button))
        );
    }
}

/**
 * @brief A function to delete a macro.
 * 
 * @param action Unused
 * @param variant_index The index of the macro to delete
 * @param data Application wide data structure
 */
static void delete_macro(GSimpleAction *action, GVariant *variant_index, app_data *data) {
    uint32_t macro_index = g_variant_get_uint32(variant_index);

    recorded_macro *macros = data->macro_data->macros;
    free(macros[macro_index].events);
    free(macros[macro_index].name);

    array_delete_element(macros, data->macro_data->macro_count, macro_index);
    
    GtkListBox *box_saved_macros = data->macro_data->box_saved_macros;
    gtk_list_box_remove(
        box_saved_macros,
        GTK_WIDGET(gtk_list_box_get_row_at_index(box_saved_macros, macro_index))
    );

    for (int i = 0; i < data->macro_data->macro_count; i++) {
        MouseMacroButton *macro_button = MOUSE_MACRO_BUTTON(
            gtk_list_box_row_get_child(gtk_list_box_get_row_at_index(box_saved_macros, i))
        );

        mouse_macro_button_set_index(macro_button, i);
    }

    update_macro_assignments(macro_index, data);
}

void assign_macro(uint32_t macro_index, byte button, app_data *data) {
    recorded_macro *macro = data->macro_data->macros + macro_index;

    macro_event *events = malloc(sizeof(macro_event) * macro->generic_event_count);
    int event_count = parse_macro(macro, events, data->macro_data->modifier_map);

    if (event_count < 0) {
        printf("Invalid macro\n");
        return;
    }

    g_mutex_lock(data->mouse->mutex);
    assign_button_macro(
        data->mouse->dev,
        button, 
        macro->repeat_mode,
        events,
        event_count
    );
    g_mutex_unlock(data->mouse->mutex);

    free(events);

    data->button_data->bindings[button] = MOUSE_ACTION_TYPE_MACRO << 8;
    data->macro_data->macro_indicies[button] = macro_index;

    gtk_menu_button_set_label(
        data->button_data->menu_button_bindings[button],
        data->macro_data->macros[macro_index].name
    );
}

/**
 * @brief A function to select a macro.
 * 
 * @param action Unused
 * @param macro_index The index of the macro
 * @param data Application wide data structure
 */
static void select_macro(GSimpleAction *action, GVariant *macro_index, app_data *data) {
    uint32_t index = g_variant_get_uint32(macro_index);
    assign_macro(index, data->button_data->selected_button, data);

    menu_button_set_popover_visibility(get_active_menu_button(data->button_data), false);
}

/**
 * @brief Gets the widgets needed for macro related config.
 * 
 * @param builder GtkBuilder object to obtain widgets
 * @param data Application wide data structure
 */
static void get_macro_data_widgets(GtkBuilder *builder, app_data *data) {
    data->macro_data->macro_key_events = GTK_EVENT_CONTROLLER(gtk_builder_get_object(builder, "macroKeyController"));
    data->macro_data->gesture_macro_mouse_events = GTK_GESTURE(gtk_builder_get_object(builder, "gestureMacroMouseEvents"));
    data->macro_data->gesture_button_save_macro_claim_click = GTK_GESTURE(gtk_builder_get_object(builder, "gestureButtonSaveMacro"));
    data->macro_data->gesture_button_record_macro_claim_click = GTK_GESTURE(gtk_builder_get_object(builder, "gestureButtonMacroRecording"));

    data->macro_data->wrap_box_macro_events = (AdwWrapBox*) adw_wrap_box_new();
    adw_wrap_box_set_child_spacing(data->macro_data->wrap_box_macro_events, 10);
    gtk_widget_set_size_request(GTK_WIDGET(data->macro_data->wrap_box_macro_events), -1, 500);
    adw_wrap_box_set_wrap_policy(data->macro_data->wrap_box_macro_events, ADW_WRAP_NATURAL);
    
    data->macro_data->box_wrap_box_macro_events = GTK_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxWrapBoxMacroEvents")));
    gtk_box_append(
        data->macro_data->box_wrap_box_macro_events,
        GTK_WIDGET(data->macro_data->wrap_box_macro_events)
    );
    
    data->macro_data->drop_down_repeat_mode = (GtkDropDown*) GTK_WIDGET(gtk_builder_get_object(builder, "dropDownRepeatMode"));

    data->macro_data->button_record_macro = GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "buttonMacroRecord")));
    data->macro_data->image_recording_macro = GTK_IMAGE(GTK_WIDGET(gtk_builder_get_object(builder, "imageRecordingMacro")));

    data->macro_data->button_save_macro = GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "buttonMacroSave")));
    
    data->macro_data->box_saved_macros = GTK_LIST_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "boxSavedMacros")));
    data->macro_data->editable_macro_name = GTK_EDITABLE(gtk_builder_get_object(builder, "editableMacroName"));
}

/**
 * @brief A function to prevent a macro event from being recorded
 * when pressing a button.
 * 
 * @param gesture The GtkGesture that emmited the event
 * @param data Unused
 */
static void claim_click(GtkGesture *gesture, void *data) {
    gtk_gesture_set_state(gesture, GTK_EVENT_SEQUENCE_CLAIMED);
}

/**
 * @brief Set up the event controllers used for listening to mouse and keyboard events. 
 * 
 * @param data Application wide data structure
 */
static void setup_event_controllers(app_data *data) {
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), data->macro_data->macro_key_events);
    g_signal_connect(data->macro_data->macro_key_events, "key-pressed", G_CALLBACK(append_macro_key_pressed), data);
    g_signal_connect(data->macro_data->macro_key_events, "key-released", G_CALLBACK(append_macro_key_released), data);
    
    gtk_widget_add_controller(GTK_WIDGET(data->widgets->window), GTK_EVENT_CONTROLLER(data->macro_data->gesture_macro_mouse_events));
    g_signal_connect(data->macro_data->gesture_macro_mouse_events, "pressed", G_CALLBACK(append_macro_mouse_pressed), data);
    g_signal_connect(data->macro_data->gesture_macro_mouse_events, "released", G_CALLBACK(append_macro_mouse_released), data);

    gtk_widget_add_controller(GTK_WIDGET(data->macro_data->button_record_macro), GTK_EVENT_CONTROLLER(data->macro_data->gesture_button_record_macro_claim_click));
    g_signal_connect(data->macro_data->gesture_button_record_macro_claim_click, "pressed", G_CALLBACK(toggle_macro_recording), data);
    
    gtk_widget_add_controller(GTK_WIDGET(data->macro_data->button_save_macro), GTK_EVENT_CONTROLLER(data->macro_data->gesture_button_save_macro_claim_click));
    g_signal_connect(data->macro_data->gesture_button_save_macro_claim_click, "pressed", G_CALLBACK(save_recorded_macro), data);
}

void app_config_macro_init(GtkBuilder *builder, app_data *data) {
    get_macro_data_widgets(builder, data);
    setup_event_controllers(data);

    const GActionEntry entries[] = {
        {.name = "add-macro",    .activate = (g_action) create_macro, .parameter_type = (const char*) G_VARIANT_TYPE_INT32},
        {.name = "select-macro", .activate = (g_action) select_macro, .parameter_type = (const char*) G_VARIANT_TYPE_UINT32},
        {.name = "edit-macro",   .activate = (g_action) edit_macro,   .parameter_type = (const char*) G_VARIANT_TYPE_UINT32},
        {.name = "delete-macro", .activate = (g_action) delete_macro, .parameter_type = (const char*) G_VARIANT_TYPE_UINT32}
    };

    g_action_map_add_action_entries(G_ACTION_MAP(data->widgets->app), entries, G_N_ELEMENTS(entries), data);
    
    recorded_macro *macros = data->macro_data->macros;
    
    for (int i = 0; i < data->macro_data->macro_count; i++) {
        create_macro_item(macros[i].name, i, data);
    }

    g_signal_connect(data->macro_data->drop_down_repeat_mode, "notify::selected-item", G_CALLBACK(change_macro_repeat_mode), data);
    widget_add_event(builder, "buttonMacroCancel", "clicked", close_macro_overlay, data);
}
