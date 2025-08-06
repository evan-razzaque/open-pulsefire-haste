#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <hidapi/hidapi.h>
#include <unistd.h>

#include "mouse.h"

void print_data(byte *data) {
	for (int i = 0; i < PACKET_SIZE; i++) {
		if (i % 16 == 0) printf("\n");
		printf("%.2x ", data[i]);
	}

	printf("\n");
}

struct hid_device_info* get_active_devices(CONNECTION_TYPE connection_type) {
	struct hid_device_info *dev_list = hid_enumerate(
		VID,
		(connection_type & CONNECTION_TYPE_WIRED)?
		PID_WIRED:
		PID_WIRELESS
	);

	return dev_list;
}

struct hid_device_info* get_devices(CONNECTION_TYPE *connection_type) {
	CONNECTION_TYPE type = 0;

	struct hid_device_info *dev_list;
	struct hid_device_info *dev_list_wired = hid_enumerate(VID, PID_WIRED);
	struct hid_device_info *dev_list_wireless = hid_enumerate(VID, PID_WIRELESS);

	if (dev_list_wired) {
		type |= CONNECTION_TYPE_WIRED;
		dev_list = dev_list_wired;

		struct hid_device_info *dev_list_last = dev_list;

		while (dev_list_last->next != NULL) {
			dev_list_last = dev_list_last->next;
		}
		
		dev_list_last->next = dev_list_wireless;
	} else {
		dev_list = dev_list_wireless;
	}

	if (dev_list_wireless) {
		type |= CONNECTION_TYPE_WIRELESS;
	}

	*connection_type = type;
	return dev_list;
}

hid_device* open_device(struct hid_device_info *dev_list) {
	struct hid_device_info *dev_info;
	hid_device *dev = NULL;

	dev_info = dev_list;

	while (dev_info) {
		if (dev_info->interface_number == INTERFACE_NUMBER) {
			dev = hid_open_path(dev_info->path);
			break;
		}

		dev_info = dev_info->next;
	}

	hid_free_enumeration(dev_list);
	return dev;
}

int mouse_write(hid_device *dev, byte *data) {
	if (dev == NULL) return -1;
	int bytes_written = hid_write(dev, data, PACKET_SIZE);

	if (bytes_written == -1) {
		printf("mouse_write: %S\n", hid_error(dev));
	}
	
	return bytes_written;
}

int mouse_send_read_request(hid_device *dev, REPORT_TYPE report_type) {
	int res;

	byte temp[PACKET_SIZE] = {REPORT_FIRST_BYTE(report_type)};
	
	res = mouse_write(dev, temp);
	
	if (res < 0) {
		printf("mouse_send_read_request: %S\n", hid_error(dev));
	}

	return res;
}

int mouse_read(hid_device *dev, byte *data) {
	if (dev == NULL) return -1;
	int res = hid_read_timeout(dev, data, PACKET_SIZE, 100);

	if (res < 0) {
		printf("mouse_read: %S\n", hid_read_error(dev));
		return -1;
	}

	return data[FIRST_BYTE];
}

int save_device_settings(hid_device *dev, color_options *color) {
	/* byte d1[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_SAVE_SETTINGS), 0x01, 0x00, 0x3c, color->red, color->green, color->blue};
	mouse_write(dev, d1);

	for (int i = 0; i < 5; i++) {
		byte d2[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_SAVE_SETTINGS_OLD), 0x01, i + 1, 0x3c};
		mouse_write(dev, d2); 
	}

	byte d3[PACKET_SIZE] = {REPORT_FIRST_BYTE(0xd9), 0x00, 0x00, 0x03, 0x55, 0x01, 0x23};
	mouse_write(dev, d3);

	byte d4[PACKET_SIZE] = {REPORT_FIRST_BYTE(0xdb), 0x55};
	mouse_write(dev, d4); */

	byte data[PACKET_SIZE] = {REPORT_FIRST_BYTE(SEND_BYTE_SAVE_SETTINGS), SAVE_BYTE_ALL};
	return mouse_write(dev, data);
}
