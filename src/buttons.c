#include <stdint.h>
#include <hidapi/hidapi.h>

#include "mouse.h"
#include "buttons.h"

int assign_button(hid_device *dev, MOUSE_BUTTON button, MOUSE_ACTION action) {
	byte type = action >> 8;

	byte data[PACKET_SIZE] = {SEND_BUTTON_ASSIGNMENT, button, type, 0x02, (byte) action, 0x00}; // TODO: Macro assignment (last byte is different)
	return mouse_write(dev, data);
}