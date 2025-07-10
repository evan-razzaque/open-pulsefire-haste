#include <gtk/gtk.h>
#include <stdint.h>

#include "macro_event_item.h"

G_DEFINE_TYPE(MacroEventItem, macro_event_item, GTK_TYPE_BOX)

static void macro_event_item_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), MACRO_TYPE_EVENT_ITEM);
    G_OBJECT_CLASS(macro_event_item_parent_class)->dispose(gobject);
}

static void macro_event_item_class_init(MacroEventItemClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = macro_event_item_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/haste/macro-event-item.ui");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);

    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, item_content);
}

static void macro_event_item_init(MacroEventItem *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MacroEventItem* macro_event_item_new(uint8_t item_type, const char* action_name, uint16_t delay) {
    MacroEventItem *self = g_object_new(MACRO_TYPE_EVENT_ITEM, NULL);

    if (item_type == MACRO_ITEM_TYPE_DELAY) {
        gtk_widget_add_css_class(GTK_WIDGET(&self->parent_instance), "delay");
        
        char delay_string[8];
        sprintf(delay_string, "%d", delay);
        gtk_label_set_text(self->item_content, delay_string);
    } else {
        gtk_widget_add_css_class(GTK_WIDGET(&self->parent_instance), "action");
        gtk_label_set_text(self->item_content, action_name);
    }

    return self;
}