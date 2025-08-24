#ifndef MOUSE_MACRO_BUTTON_H
#define MOUSE_MACRO_BUTTON_H

#include <stdint.h>
#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE(MouseMacroButton, mouse_macro_button, MOUSE, MACRO_BUTTON, GtkBox)

#define MOUSE_TYPE_MACRO_BUTTON (mouse_macro_button_get_type())
#define MOUSE_MACRO_BUTTON(inst) (G_TYPE_CHECK_INSTANCE_CAST((inst), MOUSE_TYPE_MACRO_BUTTON, MouseMacroButton))

/**
 * @brief Creates a MouseMacroButton for selecting a macro.
 * 
 * @param name The name of the macro
 * @param index The index of the macro
 * @return a new MouseMacroButton
 */
MouseMacroButton* mouse_macro_button_new(char* name, uint32_t index);

/**
 * @brief Sets the macro index for the MouseMacroButton.
 * 
 * @param self The MouseMacroButton instance
 * @param index The macro index
 */
void mouse_macro_button_set_index(MouseMacroButton* self, uint32_t index);

#endif
