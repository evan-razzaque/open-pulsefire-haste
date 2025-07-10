#ifndef MACRO_EVENT_ITEM_H
#define MACRO_EVENT_ITEM_H

#include <gtk/gtk.h>
#include <stdint.h>

enum MACRO_ITEM_TYPE {
    MACRO_ITEM_TYPE_ACTION,
    MACRO_ITEM_TYPE_DELAY
} typedef MACRO_ITEM_TYPE;

struct _MacroEventItem {
    GtkBox parent_instance;
    GtkLabel* item_content;
};

G_DECLARE_FINAL_TYPE(MacroEventItem, macro_event_item, MACRO, EVENT_ITEM, GtkBox)

#define MACRO_TYPE_EVENT_ITEM (macro_event_item_get_type())
#define MACRO_EVENT_ITEM(inst) (G_TYPE_CHECK_INSTANCE_CAST ((inst), MACRO_TYPE_EVENT_ITEM, MacroEventItem))

MacroEventItem* macro_event_item_new(uint8_t item_type, const char* action_name, uint16_t delay);

#endif