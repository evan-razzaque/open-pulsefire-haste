CC                = gcc
DEVICE_OBJFILES   = build/buttons.o build/mouse.o build/rgb.o build/sensor.o
APP_OBJFILES      = build/main.o build/mouse_config.o build/config_led.o build/config_buttons.o build/config_macro.o build/config_sensor.o build/settings_storage.o build/macro_parser.o
TEMPLATE_OBJFILES = build/dpi_profile_config.o build/stack_menu_button.o build/stack_menu_button_back.o build/mouse_macro_button.o build/macro_event_item.o
OBJFILES          = $(DEVICE_OBJFILES) $(APP_OBJFILES) $(TEMPLATE_OBJFILES) build/hotplug.o
UI_FILES          = ui/templates.gresource.xml ui/dpi-profile-config.ui ui/stack-menu-button.ui ui/stack-menu-button-back.ui ui/mouse-macro-button.ui ui/macro-event-item.ui
GRESOURCES        = resources/templates.gresource
TARGET            = bin/main

HOTPLUG_SOURCE    = src/linux/hotplug.c

ASAN = -fsanitize=address

LDLIBS  = $(ASAN) -lm -lhidapi-hidraw -lusb-1.0 $$(pkg-config --libs libadwaita-1 gmodule-export-2.0)
CFLAGS += -I src/ $$(pkg-config --cflags libadwaita-1 gmodule-export-2.0) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99 -O1 -g $(ASAN)

ifeq ($(OS),Windows_NT)
	HOTPLUG_SOURCE = src/windows/hotplug.c
	LDLIBS = -lm -lhidapi $$(pkg-config --libs libadwaita-1 gmodule-export-2.0) -I /mingw64/include/hidapi
endif

all: $(TARGET) $(GRESOURCES)
	
$(TARGET) : $(OBJFILES)
	@mkdir -p data bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS)

$(GRESOURCES) : $(UI_FILES)
	@mkdir -p resources
	glib-compile-resources $< --sourcedir ui --target $@

$(APP_OBJFILES) : src/types.h src/mouse_config.h src/util.h

# Device

build/%.o: src/device/%.c src/device/%.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

# Hotplug

build/hotplug.o: $(HOTPLUG_SOURCE) src/hotplug.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

# Application

build/main.o: src/main.c src/hid_keyboard_map.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/config_macro.o build/macro_event_item.o build/macro_parser.o: src/macro_types.h

build/macro_parser.o: src/macro_parser.c src/macro_parser.h src/macro_types.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/%.o: src/%.c
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

# Templates

build/main.o: src/templates/stack_menu_button.h src/templates/stack_menu_button_back.h
build/config_sensor.o: src/templates/dpi_profile_config.h
build/config_macro.o: src/templates/macro_event_item.h

build/%.o: src/templates/%.c src/templates/%.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

# Clean

clean:
	rm -rf bin build resources data

clean-partial:
	rm -rf bin build resources

clean-data:
	rm -rf data/*

clean-macros:
	rm data/macros.bin

clean-settings:
	rm data/mouse_settings.bin