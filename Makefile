CC                 = gcc
DEVICE_OBJFILES    = build/buttons.o build/mouse.o build/rgb.o build/sensor.o
APP_OBJFILES       = build/main.o build/mouse_config.o build/config_led.o build/config_buttons.o build/config_macro.o build/config_sensor.o build/settings_storage.o build/macro_parser.o
TEMPLATE_OBJFILES  = build/dpi_profile_config.o build/stack_menu_button.o build/stack_menu_button_back.o build/mouse_macro_button.o build/macro_event_item.o
GRESOURCES_OBJFILE = build/gresources.o
UI_FILES           = ui/gresources.gresource.xml ui/window.ui ui/window.css ui/dpi-profile-config.ui ui/stack-menu-button.ui ui/stack-menu-button-back.ui ui/mouse-macro-button.ui ui/macro-event-item.ui
GRESOURCES         = resources/templates.gresource
TARGET             = bin/main

HOTPLUG_OBJ        = build/hotplug_linux.o
ASAN               = -fsanitize=address

LOCAL_LIB          =/usr/local/lib

# libadwaita wip/alice/wrap-layout-leak
LIBADWAITA         = $(LOCAL_LIB)/pkgconfig/libadwaita-1.pc

LDLIBS  = -g $(ASAN) -lm -lhidapi-hidraw -lusb-1.0 $$(pkg-config --libs $(LIBADWAITA) gmodule-export-2.0) -Wl,-rpath,$(LOCAL_LIB)
CFLAGS += -g $(ASAN)  -Isrc/ $$(pkg-config --cflags $(LIBADWAITA) gmodule-export-2.0) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99 -Og

ifeq ($(OS),Windows_NT)
	ASAN=
	HOTPLUG_OBJ = build/hotplug_windows.o
	LDLIBS      = -lm -lhidapi -lhid -lcfgmgr32 $$(pkg-config --libs libadwaita-1 gmodule-export-2.0) -I /mingw64/include/hidapi
endif

OBJFILES = $(DEVICE_OBJFILES) $(GRESOURCES_OBJFILE) $(APP_OBJFILES) $(TEMPLATE_OBJFILES) $(HOTPLUG_OBJ) build/hotplug_common.o
VPATH    = resources src src/device src/hotplug src/templates

all: $(TARGET)
	
$(TARGET) : $(OBJFILES)
	@mkdir -p data bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS)

build/config_buttons.o: config_buttons.h types.h device/buttons.h device/rgb.h util.h
build/sensor.o: sensor.h mouse.h
build/buttons.o: buttons.h mouse.h
build/rgb.o: rgb.h mouse.h
build/mouse.o: mouse.h
build/main.o: types.h device/mouse.h device/rgb.h device/buttons.h device/sensor.h hotplug/hotplug.h settings_storage.h hid_keyboard_map.h mouse_config.h config_led.h config_buttons.h config_macro.h config_sensor.h templates/stack_menu_button.h templates/stack_menu_button_back.h templates/mouse_macro_button.h templates/gresources.h util.h
build/hotplug_windows.o: hotplug/hotplug.h
build/hotplug_linux.o: hotplug/hotplug.h device/mouse.h types.h
build/hotplug_common.o: hotplug/hotplug_common.h
build/mouse_config.o: mouse_config.h types.h config_buttons.h
build/settings_storage.o: settings_storage.h device/rgb.h device/mouse.h device/buttons.h device/sensor.h types.h config_led.h config_buttons.h config_macro.h config_sensor.h
build/config_led.o: config_led.h types.h device/rgb.h
build/dpi_profile_config.o: dpi_profile_config.h
build/stack_menu_button_back.o: stack_menu_button_back.h
build/mouse_macro_button.o: mouse_macro_button.h
build/stack_menu_button.o: stack_menu_button.h
build/macro_event_item.o: macro_event_item.h
build/config_macro.o: config_macro.h device/buttons.h types.h util.h mouse_config.h config_buttons.h macro_parser.h templates/mouse_macro_button.h templates/macro_event_item.h
build/config_sensor.o: config_sensor.h device/mouse.h device/sensor.h types.h util.h templates/dpi_profile_config.h
build/macro_parser.o: macro_parser.h config_macro.h hid_keyboard_map.h

build/%.o : %.c 
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

src/templates/gresources.h: resources/gresources.c

resources/gresources.c : $(UI_FILES)
	@mkdir -p resources
	glib-compile-resources $< --sourcedir ui --target resources/gresources.c --generate-source
	glib-compile-resources $< --sourcedir ui --target src/templates/gresources.h --generate-header

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
	