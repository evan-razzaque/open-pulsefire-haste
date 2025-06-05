#include <stdint.h>
#include <hidapi/hidapi.h>

#include "mouse.h"

hid_device* open_device() {
	struct hid_device_info *dev_list, *dev_info;
	hid_device *dev;

	dev_list = hid_enumerate(VID, PID_WIRELESS);

	if (!dev_list) {
		dev_list = hid_enumerate(VID, PID_WIRED);
        if (!dev_list) return NULL;
	}

	dev_info = dev_list;

	while (dev_info) {
		if (dev_info->interface_number == INTERFACE) {
			dev = hid_open_path(dev_info->path);
			break;
		}

		dev_info = dev_info->next;
	}

	hid_free_enumeration(dev_list);
	return dev;
}

int mouse_write(hid_device *dev, uint8_t *data) {
	return hid_write(dev, data, PACKET_SIZE);
}

int mouse_read(hid_device *dev, MOUSE_REPORT reportType, uint8_t *data) {
	int res;

	data[0] = reportType;

	res = hid_write(dev, data, PACKET_SIZE);
	if (res < 0) return res;

	data[0] = 0x00;
	
	return hid_read(dev, data, PACKET_SIZE);
}

int save_settings(hid_device *dev) {
	uint8_t data[PACKET_SIZE] = {SAVE_SETTINGS, 0xff};

	return mouse_write(dev, data);
}