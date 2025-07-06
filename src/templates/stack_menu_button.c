#include <gtk/gtk.h>
#include <string.h>

#include "stack_menu_button.h"

G_DEFINE_TYPE(StackMenuButton, stack_menu_button, GTK_TYPE_BUTTON)

enum {
    PROP_0,
    PROP_DIRECTION,
    PROP_HAS_ICON,
};

static void stack_menu_button_change_direction(StackMenuButton *self, const char* direction) {
    if (strcmp("left", direction) != 0) return;

    GtkBox *box = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(self->label)));
    gtk_box_remove(box, GTK_WIDGET(self->label));
    gtk_box_append(box, GTK_WIDGET(self->label));

    gtk_image_set_from_icon_name(self->image, "go-previous");
    gtk_widget_set_halign(GTK_WIDGET(self->image), GTK_ALIGN_START);
}

static void stack_menu_button_set_property(GObject *object, uint32_t prop_id, const GValue* value, GParamSpec *param_spec) {
    StackMenuButton *stack_menu_button = STACK_MENU_BUTTON(object);

    switch (prop_id) {
    case PROP_DIRECTION:
        stack_menu_button_change_direction(stack_menu_button, g_value_get_string(value));
        break;
    case PROP_HAS_ICON:
        stack_menu_button->has_icon = g_value_get_boolean(value);
        gtk_widget_set_opacity(GTK_WIDGET(stack_menu_button->image), (double) (int) stack_menu_button->has_icon);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
        break;
    }
}

static void stack_menu_button_get_property(GObject *object, uint32_t prop_id, GValue* value, GParamSpec *param_spec) {
    StackMenuButton *stack_menu_button = STACK_MENU_BUTTON(object);

    switch (prop_id) {
    case PROP_DIRECTION:
        g_value_set_string(value, stack_menu_button->direction);
        break;
    case PROP_HAS_ICON:
        g_value_set_boolean(value, stack_menu_button->has_icon);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, param_spec);
        break;
    }
}

static void stack_menu_button_dispose(GObject *gobject) {
    gtk_widget_dispose_template(GTK_WIDGET(gobject), STACK_TYPE_MENU_BUTTON);
    G_OBJECT_CLASS(stack_menu_button_parent_class)->dispose(gobject);
}

static void stack_menu_button_class_init(StackMenuButtonClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = stack_menu_button_dispose;
    G_OBJECT_CLASS(klass)->set_property = stack_menu_button_set_property;
    G_OBJECT_CLASS(klass)->get_property = stack_menu_button_get_property;
    
    // GParamSpec *param_specs[2] = {
    //     g_param_spec_string("direction", "Direction", "Direction of the button.", "right", G_PARAM_READWRITE),
    //     g_param_spec_boolean("has-icon", "icon-visibility", "Whether to show the arrow icon or not.", true, G_PARAM_READWRITE)
    // };
    
    // g_object_class_install_properties(G_OBJECT_CLASS(klass), G_N_ELEMENTS(param_specs), (GParamSpec**) param_specs);

    g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_DIRECTION, g_param_spec_string("direction", "Direction", "Direction of the button.", "right", G_PARAM_READWRITE));
    g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_HAS_ICON,  g_param_spec_boolean("has-icon", "icon-visibility", "Whether to show the arrow icon or not.", true, G_PARAM_READWRITE));
    
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/com/haste/stack-menu-button.ui");

    gtk_widget_class_bind_template_child(widget_class, StackMenuButton, label);
    gtk_widget_class_bind_template_child(widget_class, StackMenuButton, image);
}

static void stack_menu_button_init(StackMenuButton *self) {
    self->direction = "right";
    self->has_icon = true;
    gtk_widget_init_template(GTK_WIDGET(self));
}