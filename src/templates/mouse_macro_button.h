#include <gtk/gtk.h>

struct _MouseMacroButton {
    GtkBox parent_instance;

    GtkButton *button_name; 
    GtkButton *button_edit;
    GtkButton *button_delete;
};

G_DECLARE_FINAL_TYPE(MouseMacroButton, mouse_macro_button, MOUSE, MACRO_BUTTON, GtkBox)

#define MOUSE_TYPE_MACRO_BUTTON (mouse_macro_button_get_type())
#define MOUSE_MACRO_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MOUSE_TYPE_MACRO_BUTTON, MouseMacroButton))

/**
 * @brief Creates a MouseMacroButton for selecting a macro.
 * 
 * @param name The name of the macro
 * @param index The index of the macro
 * @return a nes MouseMacroButton
 */
MouseMacroButton* mouse_macro_button_new(char* name, int index);
