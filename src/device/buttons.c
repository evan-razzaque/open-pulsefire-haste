#include <stdio.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <math.h>

#include "mouse.h"
#include "buttons.h"

#define min(a, b) (((a) < (b))? (a) : (b))

int assign_button_action(hid_device *dev, MOUSE_BUTTON button, uint16_t action) {
	byte type = action >> 8;
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_BUTTON_ASSIGNMENT), button, type, 0x02, action};
	
	return mouse_write(dev, data);
}

static int set_macro_assignment(hid_device *dev, MOUSE_BUTTON button, MACRO_REPEAT_MODE repeat_mode, int event_count) {
	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_MACRO_ASSIGNMENT), button, 0x00, 0x05, event_count, 0x00, repeat_mode};

	// print_data(data);
	return mouse_write(dev, data);
}

int assign_button_macro(hid_device *dev, MACRO_BINDING binding, MACRO_REPEAT_MODE repeat_mode, macro_event *events, int event_count) {
	byte button = binding;
	int res = 0;

	res = assign_button_action(dev, button, binding);

	byte data[PACKET_SIZE] = {REPORT_BYTE(SEND_BYTE_MACRO_DATA), button, 0x00, event_count};

	int events_remaining = event_count;
	int packet_count = ceil(event_count / 6.0);
	int i = 0;

	for (int p = 0; p < packet_count; p++) {
		data[FIRST_BYTE + 2] = MACRO_PACKET_SUM(p);
		data[FIRST_BYTE + 3] = min(events_remaining, 6) + MACRO_PACKET_EVENT_COUNT(p);

		// Since mouse events are in pairs and keyboard events are not,
		// we must keep track of how many generic events can fit in a single packet to
		// ensure we don't put extra events in a packet.
		int generic_event_limit = 6;

		for (int j = 0; j < generic_event_limit && events_remaining > 0; j++) {
			/**
			 * Since the first byte of a macro event is the macro type,
			 * and MACRO_EVENT_TYPE_KEYBOARD = 0x1a and MACRO_EVENT_TYPE_MOUSE = 0x25,
			 * we can shift the byte right 4 bits to obtain the actual number of events.
			 */
			int actual_event_count = events[i].event_data[0] >> 4;
			
			events_remaining -= actual_event_count;
			generic_event_limit -= actual_event_count - 1;
			
			int offset = (FIRST_BYTE + 4) + (MACRO_EVENT_SIZE * j);
			memcpy(data + offset, events[i].event_data, MACRO_EVENT_SIZE);
			i++;
		}

		// print_data(data);
		res = mouse_write(dev, data);
	}

	res = set_macro_assignment(dev, button, repeat_mode, event_count);

	return res;
}