#include <gtk/gtk.h>
#include <stdint.h>

#include "macro_event_item.h"

#define ICON_DOWN_ARROW ("go-down")
#define ICON_UP_ARROW ("go-up")

struct _MacroEventItem {
    GtkBox parent_instance;
    GtkLabel *label_action_name, *label_delay;
    GtkImage *image_action_type;
};

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

    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, label_action_name);
    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, label_delay);
    gtk_widget_class_bind_template_child(widget_class, MacroEventItem, image_action_type);
}

static void macro_event_item_init(MacroEventItem *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

MacroEventItem* macro_event_item_new(const char* action_name, uint16_t delay, MACRO_EVENT_TYPE event_type) {
    MacroEventItem *self = g_object_new(MACRO_TYPE_EVENT_ITEM, NULL);

    if (delay > 0) {
        char delay_string[8];
        sprintf(delay_string, "%d", delay);
        gtk_label_set_text(self->label_delay, delay_string);
    }

    gtk_label_set_text(self->label_action_name, action_name);

    gtk_image_set_from_icon_name(
        self->image_action_type, 
        (event_type == MACRO_EVENT_TYPE_DOWN)? ICON_DOWN_ARROW : ICON_UP_ARROW
    );
    
    return self;
}
