/*
 * This file is part of the open-pulsefire-haste project
 * Copyright (C) 2025  Evan Razzaque
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <hidapi/hidapi.h>
#include <string.h>
#include <math.h>

#include "buttons.h"
#include "mouse.h"

#define MIN(a, b) (((a) < (b))? (a) : (b))

int assign_button_action(hid_device *dev, MOUSE_BUTTON button, mouse_action action) {
	return mouse_write(dev, (byte[PACKET_SIZE]) {
		REPORT_FIRST_BYTE(SEND_BYTE_BUTTON_ASSIGNMENT),
		button,
		action >> 8,
		0x02,
		action
	});
}

int assign_button_macro(hid_device *dev, MOUSE_BUTTON button, REPEAT_MODE repeat_mode, macro_event *events, int event_count) {
	int res;

	res = assign_button_action(dev, button, (MOUSE_ACTION_TYPE_MACRO << 8) + button);
	if (res < 0) return res;

	byte data[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_MACRO_DATA), button, 0x00, event_count};

	int events_remaining = event_count;
	int packet_count = ceil(event_count / 6.0);
	int i = 0;

	// Macro data packets
	for (int p = 0; p < packet_count; p++) {
		data[FIRST_BYTE + 2] = MACRO_PACKET_SUM(p);
		data[FIRST_BYTE + 3] = MIN(events_remaining, 6) + MACRO_PACKET_EVENT_COUNT(p);

		int offset = FIRST_BYTE + 4;

		// Macro events
		for (int j = 6; j > 0 && events_remaining > 0;) {
			int actual_event_count = events[i].action_type >> 4;

			events_remaining -= actual_event_count;
			j -= actual_event_count;

			memcpy(data + offset, events[i].event_data, MACRO_EVENT_SIZE);
			offset += MACRO_EVENT_SIZE;
			i++;
		}

		// Zeros out unessesary event bytes from previous packets
		memset(data + offset, 0, PACKET_SIZE - offset);

		res = mouse_write(dev, data);
		if (res < 0) return res;
	}

	return mouse_write(dev, (byte[PACKET_SIZE]) {
		REPORT_FIRST_BYTE(SEND_BYTE_MACRO_ASSIGNMENT),
		button,
		0x00,
		0x05,
		event_count,
		0x00,
		repeat_mode
	});
}
