#define MACRO_PARSER_PRIVATE
#include "macro_parser.h"
#include "macro_types.h"
#include "mouse_config.h"
#include "hid_keyboard_map.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct macro_parser_data {
    mouse_macro *recored_macro; // Macro recored by the user
    macro_event *events; // Stores the macro events to write to the mouse
    
    int event_count, event_index;
    bool keys_down[256], event_keys[256];
    int keys_down_count, event_keys_count;
    bool is_mouse_down;
} typedef macro_parser_data;

static void parse_key_down_event(macro_parser_data *p, byte *modifier_map, generic_macro_event *generic_event) {
    if (p->keys_down_count == 0) {
        p->events[p->event_index] = KEYBOARD_EVENT_DOWN(0, generic_event->delay_next_action, 0);
        p->event_count++;
    } else if (p->event_keys_count < 6) {
        int previous_delay = p->events[p->event_index].key_event.delay_next_action;
        p->events[p->event_index].key_event.delay_next_action = MAX(previous_delay, generic_event->delay_next_action);
    } else {
        return;
    }
    
    p->event_keys[generic_event->action] = true;
    p->keys_down[generic_event->action] = true;
    p->keys_down_count++;

    if (generic_event->action >= LCTRL) {
        p->events[p->event_index].key_event.modifier_keys += modifier_map[generic_event->action];
        return;
    }

    p->events[p->event_index].key_event.keys[p->event_keys_count] = generic_event->action;
    p->event_keys_count++;
}

static void parse_key_up_event(macro_parser_data *p, generic_macro_event *generic_event) {
    p->keys_down[generic_event->action] = false;
    p->keys_down_count--;
    
    if (p->keys_down_count > 0) return;
    p->event_count++;

    p->event_index++;
    p->events[p->event_index] = KEYBOARD_EVENT_UP(generic_event->delay_next_action);
    p->event_index++;
    
    p->event_keys_count = 0;
    memset(&p->keys_down, false, sizeof(p->keys_down));
    memset(&p->event_keys, false, sizeof(p->event_keys));
}

static void parse_mouse_event(macro_parser_data *p, generic_macro_event *generic_event, MACRO_ACTION_TYPE action_type) {
    p->event_count++;
    p->is_mouse_down = (bool) action_type;

    if (action_type == MACRO_ACTION_TYPE_DOWN) {
        p->events[p->event_index] = MOUSE_EVENT(generic_event->action, generic_event->delay_next_action, 0);
    } else {
        p->events[p->event_index].mouse_event.up.delay_next_action = generic_event->delay_next_action;
        p->event_index++;
    }
}

int parse_macro(mouse_macro *macro, macro_event *events, byte *modifier_map) {
    macro_parser_data parser_data = {
        .recored_macro = macro,
        .events = events,
    };
    
    bool *keys_down = parser_data.keys_down;
    bool *event_keys = parser_data.event_keys;

    for (int i = 0; i < macro->generic_event_count; i++) {
        generic_macro_event generic_event = macro->events[i];
        uint16_t event_action_type = (uint16_t) (generic_event.event_type << 8) + (uint16_t) generic_event.action_type; 

        switch (event_action_type) {
        case KEY_DOWN:
            if (parser_data.is_mouse_down || event_keys[generic_event.action]) break;
            parse_key_down_event(&parser_data, modifier_map, &generic_event);
            break;
        case KEY_UP:
            bool event_has_key = event_keys[generic_event.action] && keys_down[generic_event.action];
            if (parser_data.is_mouse_down || !event_has_key) break;

            parse_key_up_event(&parser_data, &generic_event);
            break;
        case MOUSE_DOWN:
        case MOUSE_UP: 
            if (parser_data.keys_down_count > 0) break;
            parse_mouse_event(&parser_data, &generic_event, generic_event.action_type);
            break;
        default:
            printf("%.4x is not a valid event_action_type\n", event_action_type);
            exit(-1);
            break;
        }
    }

    if (parser_data.is_mouse_down || parser_data.keys_down_count > 0) return -1;
    return parser_data.event_count;
}