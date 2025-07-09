#include <gtk/gtk.h>

struct _MouseMacroButton {
    GtkBox parent_instance;

    GtkButton *button_name, *button_edit, *button_delete;
};

G_DECLARE_FINAL_TYPE(MouseMacroButton, mouse_macro_button, MOUSE, MACRO_BUTTON, GtkBox)

#define MOUSE_TYPE_MACRO_BUTTON (mouse_macro_button_get_type())
#define MOUSE_MACRO_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST ((inst), MOUSE_TYPE_MACRO_BUTTON, MouseMacroButton))