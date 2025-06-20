#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <math.h>

#include "mouse.h"
#include "buttons.h"

int assign_button_action(hid_device *dev, MOUSE_BUTTON button, uint16_t action) {
	byte type = action >> 8;
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_BUTTON_ASSIGNMENT), button, type, 0x02, action};
	
	return mouse_write(dev, data);
}

static int set_macro_assignment(hid_device *dev, MOUSE_BUTTON button, MACRO_REPEAT_MODE repeat_mode, int event_count) {
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_MACRO_ASSIGNMENT), button, 0x00, 0x05, event_count, 0x00, repeat_mode};

	print_data(data);
	return mouse_write(dev, data);
}

int assign_button_macro(hid_device *dev, MACRO_BINDING binding, MACRO_REPEAT_MODE repeat_mode, macro_event *events, int event_count) {
	byte button = binding;
	int res;

	res = assign_button_action(dev, button, binding);

	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_MACRO_DATA), button, 0x00, event_count};

	int events_remaining = event_count;
	int packet_count = ceil(event_count / 6.0);

	for (int i = 0; i < packet_count; i++) {
		data[FIRST_BYTE + 2] = MACRO_PACKET_SUM(i);
		data[FIRST_BYTE + 3] = ((events_remaining < 6)? events_remaining : 6) + MACRO_PACKET_EVENT_COUNT(i);

		for (int j = 0; j < 6 && events_remaining > 0; j++) {
			int offset = (FIRST_BYTE + 4) + (MACRO_EVENT_SIZE * j);

			events_remaining--;
			memcpy(data + offset, events[j].event_data, MACRO_EVENT_SIZE);
		}

		print_data(data);
		res = mouse_write(dev, data);
	}

	res = set_macro_assignment(dev, button, repeat_mode, event_count);

	return res;
}