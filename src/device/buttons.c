#include <stdint.h>
#include <hidapi/hidapi.h>

#include "mouse.h"
#include "buttons.h"

int assign_button_action(hid_device *dev, MOUSE_BUTTON button, uint16_t action) {
	byte type = action >> 8;
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_BUTTON_ASSIGNMENT), button, type, 0x02, action, 0x00}; // TODO: Macro assignment (last byte is different)
	return mouse_write(dev, data);
}