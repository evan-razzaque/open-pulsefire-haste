CC                 = gcc
DEVICE_OBJFILES    = build/buttons.o build/mouse.o build/rgb.o build/sensor.o
APP_OBJFILES       = build/main.o build/mouse_config.o build/config_led.o build/config_buttons.o build/config_macro.o build/config_sensor.o build/settings_storage.o build/macro_parser.o
TEMPLATE_OBJFILES  = build/dpi_profile_config.o build/stack_menu_button.o build/stack_menu_button_back.o build/mouse_macro_button.o build/macro_event_item.o
GRESOURCES_OBJFILE = build/gresources.o
OBJFILES           = $(DEVICE_OBJFILES) $(APP_OBJFILES) $(TEMPLATE_OBJFILES) $(GRESOURCES_OBJFILE) build/hotplug.o build/hotplug_common.o
UI_FILES           = ui/gresources.gresource.xml ui/window.ui ui/window.css ui/dpi-profile-config.ui ui/stack-menu-button.ui ui/stack-menu-button-back.ui ui/mouse-macro-button.ui ui/macro-event-item.ui
GRESOURCES         = resources/templates.gresource
TARGET             = bin/main

HOTPLUG_SOURCE     = src/hotplug/hotplug_linux.c
ASAN               = -fsanitize=address

LOCAL_LIB=/usr/local/lib

# libadwaita wip/alice/wrap-layout-leak
LIBADWAITA         = $(LOCAL_LIB)/pkgconfig/libadwaita-1.pc

LDLIBS  = -g $(ASAN) -lm -lhidapi-hidraw -lusb-1.0 $$(pkg-config --libs $(LIBADWAITA) gmodule-export-2.0) -Wl,-rpath,$(LOCAL_LIB)
CFLAGS += -g $(ASAN) -I src/ -I resources/ $$(pkg-config --cflags $(LIBADWAITA) gmodule-export-2.0) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99 -Og

ifeq ($(OS),Windows_NT)
	ASAN=
	HOTPLUG_SOURCE = src/hotplug/hotplug_windows.c
	LDLIBS = -lm -lhidapi -lhid -lcfgmgr32 $$(pkg-config --libs libadwaita-1 gmodule-export-2.0) -I /mingw64/include/hidapi
endif

VPATH = resources src src/device src/hotplug src/templates

all: $(TARGET) 
	
$(TARGET) : $(OBJFILES)
	@mkdir -p data bin
	$(CC) -o $(TARGET) $(OBJFILES) $(LDLIBS) $(LDFLAGS)

build/main.o : src/main.c
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/hotplug.o : $(HOTPLUG_SOURCE) hotplug.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/%.o : %.c %.h
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

resources/gresources.c : $(UI_FILES)
	@mkdir -p resources
	glib-compile-resources $< --sourcedir ui --target resources/gresources.c --generate

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