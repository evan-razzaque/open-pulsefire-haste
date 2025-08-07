CC             = gcc
UI_FILES       = ui/gresources.gresource.xml ui/window.ui ui/window.css ui/dpi-profile-config.ui ui/stack-menu-button.ui ui/stack-menu-button-back.ui ui/mouse-macro-button.ui ui/macro-event-item.ui
BUILD_DIR      = build
BIN_DIR        = bin
NAME           = main
TARGET         = $(BIN_DIR)/$(NAME)

HOTPLUG_SRC    = src/hotplug/hotplug_linux.c
GRESOURCES_SRC = resources/gresources.c
GRESOURCES_HEADER = resources/gresources.h
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

GEN_DIRS = resources data $(DEPDIR) $(BUILD_DIR) $(BIN_DIR)

all: $(TARGET)
	
$(TARGET) : $(GEN_DIRS) $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

$(GRESOURCES_OBJ): $(GRESOURCES_SRC)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/main.o : $(GRESOURCES_HEADER)

$(BUILD_DIR)/%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(CC) $(DEPFLAGS) -Isrc $<
	$(CC) -c $(CFLAGS) $< -o $@

$(GRESOURCES_HEADER):
	glib-compile-resources ui/gresources.gresource.xml --sourcedir ui --target $@ --generate-header
	
$(GRESOURCES_SRC) : $(UI_FILES)
	glib-compile-resources $< --sourcedir ui --target $@ --generate-source

$(GEN_DIRS): 
	mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

clean:
	rm -rf $(GEN_DIRS)

clean-partial:
	rm -rf $(BIN_DIR) $(BUILD_DIR) $(DEPDIR) resources

clean-data:
	rm -rf data/*

clean-macros:
	rm data/macros.bin

clean-settings:
	rm data/mouse_settings.bin
	