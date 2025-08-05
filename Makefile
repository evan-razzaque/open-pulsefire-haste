CC             = gcc
UI_FILES       = ui/gresources.gresource.xml ui/window.ui ui/window.css ui/dpi-profile-config.ui ui/stack-menu-button.ui ui/stack-menu-button-back.ui ui/mouse-macro-button.ui ui/macro-event-item.ui
BUILD_DIR      = build
TARGET         = bin/main

HOTPLUG_SRC    = src/hotplug/hotplug_linux.c
GRESOURCES_SRC = resources/gresources.c
GRESOURCES_OBJ = $(BUILD_DIR)/gresources.o

LOCAL_LIB      = /usr/local/lib

# libadwaita wip/alice/wrap-layout-leak
LIBADWAITA     = $(LOCAL_LIB)/pkgconfig/libadwaita-1.pc

DEBUG    = -fsanitize=address -g -Og
LDLIBS   = $(DEBUG) -lm -lhidapi-hidraw -lusb-1.0 $$(pkg-config --libs $(LIBADWAITA) gmodule-export-2.0) -Wl,-rpath,$(LOCAL_LIB)
CFLAGS  += $(DEBUG) -Isrc/ $$(pkg-config --cflags $(LIBADWAITA) gmodule-export-2.0) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99

ifeq ($(OS),Windows_NT)
	DEBUG=
	HOTPLUG_SRC  = src/hotplug/hotplug_windows.c
	LDLIBS      = -lm -lhidapi -lhid -lcfgmgr32 $$(pkg-config --libs libadwaita-1 gmodule-export-2.0) -I /mingw64/include/hidapi
endif

VPATH    = resources src src/device src/hotplug src/templates

SRCS    := $(shell basename -a $$(find src/ -name "*.c" ! -name hotplug_linux.c ! -name hotplug_windows.c) $(HOTPLUG_SRC) $(GRESOURCES_SRC)) 
OBJS    := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS)) 
DEPDIR   = $(BUILD_DIR)/.deps
DEPFLAGS = -MT $@ -MM -MP -MF $(DEPDIR)/$*.d

all: $(TARGET)
	
$(TARGET) : $(OBJS)
	@mkdir -p data bin
	$(CC) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

$(GRESOURCES_OBJ): $(GRESOURCES_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(DEPFLAGS) -Isrc $<
	$(CC) -c $(CFLAGS) $< -o $@

src/templates/gresources.h: $(UI_FILES) $(GRESOURCES_SRC)
	glib-compile-resources $< --sourcedir ui --target $@ --generate-header

$(GRESOURCES_SRC) : $(UI_FILES)
	@mkdir -p resources
	glib-compile-resources $< --sourcedir ui --target $@ --generate-source

$(DEPDIR): 
	@mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

clean:
	rm -rf bin $(BUILD_DIR) resources data

clean-partial:
	rm -rf bin $(BUILD_DIR) resources

clean-data:
	rm -rf data/*

clean-macros:
	rm data/macros.bin

clean-settings:
	rm data/mouse_settings.bin
	