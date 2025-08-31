CC                = gcc
BUILD_DIR         = build
BIN_DIR           = bin
NAME              = main
TARGET            = $(BIN_DIR)/$(NAME)

VPATH             = src src/device src/hotplug src/templates

RESOURCES         := $(shell find ui -name *.ui -o -name *.css)
GRESOURCE_BUNDLE  = ui/gresources.gresource.xml 
GRESOURCES_SRC    = resources/gresources.c
GRESOURCES_HEADER = resources/gresources.h
GRESOURCES_OBJ    = $(BUILD_DIR)/gresources.o

ifeq ($(debug), 1)
	DEBUG_FLAGS = -fsanitize=address -g -Og
else
	debug = 0
endif

# Checks if the program build mode matches the user-specified build mode
ACTUAL_TARGET := $(shell ./check_build_mode.sh $(BIN_DIR)/$(NAME) $(debug))

LDLIBS  = $(DEBUG_FLAGS) -lm -lhidapi-hidraw -lusb-1.0 $$(pkg-config --libs libadwaita-1 gmodule-export-2.0)
CFLAGS += $(DEBUG_FLAGS) -Isrc/ $$(pkg-config --cflags libadwaita-1 gmodule-export-2.0) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99

ifeq ($(OS),Windows_NT)
# Asan isn't available with gcc on Windows
	DEBUG_FLAGS =
	LDLIBS      = -lm -lhidapi -lhid -lcfgmgr32 $$(pkg-config --libs libadwaita-1 gmodule-export-2.0) -I /mingw64/include/hidapi
	VPATH      += src/windows
else
	VPATH      += src/linux
endif

SRCS    := $(shell basename -a $$(find $(VPATH) -maxdepth 1 -name "*.c") $(GRESOURCES_SRC))
OBJS    := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPDIR   = $(BUILD_DIR)/.deps
DEPFLAGS = -MT $@ -MM -MP -MF $(DEPDIR)/$*.d
DEPS    := $(SRCS:%.c=$(DEPDIR)/%.d)

# The find command in SRCS will error if resources/ does not exist
VPATH += resources

GEN_DIRS = resources $(DEPDIR) $(BUILD_DIR) $(BIN_DIR)

all: $(ACTUAL_TARGET)
	
$(TARGET): $(GEN_DIRS) $(GRESOURCES_OBJ) $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

$(BUILD_DIR)/%.o : %.c
	$(CC) $(DEPFLAGS) -Isrc $<
	$(CC) -c $(CFLAGS) $< -o $@

$(GEN_DIRS): 
	mkdir -p $@

# Generated files

$(GRESOURCES_OBJ): $(GRESOURCES_SRC) $(GRESOURCES_HEADER)
	$(CC) -c $(CFLAGS) $< -o $@

$(GRESOURCES_HEADER): $(GRESOURCE_BUNDLE)
	glib-compile-resources $< --sourcedir ui --target $@ --generate-header
	
$(GRESOURCES_SRC): $(GRESOURCE_BUNDLE) $(RESOURCES)
	glib-compile-resources $< --sourcedir ui --target $@ --generate-source


.PHONY: all clean build_mode_mismatch_debug build_mode_mismatch_release

build_mode_mismatch_release:
	@echo Specified build mode \(release\) does not match target build mode \(debug\)
	@test

build_mode_mismatch_debug:
	@echo Specified build mode \(debug\) does not match target build mode \(release\)
	@test

clean:
	rm -rf $(GEN_DIRS)

-include $(wildcard $(DEPS))