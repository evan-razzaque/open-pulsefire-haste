CC                = gcc
BUILD_DIR         = build
BIN_DIR           = bin
NAME              = main
TARGET            = $(BIN_DIR)/$(NAME)

VPATH             = src src/device src/hotplug src/templates

RESOURCES        := $(shell find ui -name *.ui -o -name *.css)
GRESOURCE_BUNDLE  = ui/gresources.gresource.xml
GRESOURCES_SRC    = resources/gresources.c
GRESOURCES_HEADER = resources/gresources.h
GRESOURCES_OBJ    = $(BUILD_DIR)/gresources.o

ifeq (debug,$(filter debug,$(MAKECMDGOALS)))
	DEBUG_FLAGS = -fsanitize=address -g -Og
	debug = 1
else
	debug = 0
endif

BUILD_MODE = $(shell grep __asan_init $(TARGET) &> /dev/null; echo $$?)

# Ensures debug != grep, since grep uses 0 for success, 1 for failure, and 2 for file (target) not found
ifneq ($(debug),$(BUILD_MODE))
	ACTUAL_TARGET := $(TARGET)
else
	ACTUAL_TARGET := build_mode_mismatch_debug$(debug)
endif

# Prevents headers from pkg-config from being included in make dependencies
PKG_CONFIG_HEADERS := $(subst -I,-isystem ,$(shell pkg-config --cflags libadwaita-1 gmodule-export-2.0))

LDLIBS  = $(DEBUG_FLAGS) -lm -lhidapi-hidraw -lusb-1.0 $(shell pkg-config --libs libadwaita-1 gmodule-export-2.0)
CFLAGS += $(DEBUG_FLAGS) -Isrc/ $(PKG_CONFIG_HEADERS) -Wall -Werror -Werror=vla -Wno-deprecated-declarations -std=c99

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
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
DEPS    := $(SRCS:%.c=$(DEPDIR)/%.d)

# The find command in SRCS will error if resources/ does not exist
VPATH += resources

GEN_DIRS = resources $(DEPDIR) $(BUILD_DIR) $(BIN_DIR)

# Required by application.c, this ensures gresources.h is created first when make is parallel
$(shell \
	if [ ! -d resources -a ! -f $(GRESOURCES_HEADER) ]; then \
		mkdir resources; \
		glib-compile-resources $(GRESOURCE_BUNDLE) --sourcedir ui --target $(GRESOURCES_HEADER) --generate-header; \
	fi \
)

all: release
release: $(ACTUAL_TARGET)
debug: $(ACTUAL_TARGET)
setup:
	$(MAKE) $(TARGET) -Bn --no-print-directory

$(TARGET): $(GEN_DIRS) $(GRESOURCES_OBJ) $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

$(BUILD_DIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $(DEPFLAGS) $< -o $@

$(GEN_DIRS):
	mkdir -p $@

# Generated files

$(GRESOURCES_OBJ): $(GRESOURCES_SRC) $(GRESOURCES_HEADER)
	$(CC) -c $(CFLAGS) $< -o $@

$(GRESOURCES_HEADER): $(GRESOURCE_BUNDLE)
	glib-compile-resources $< --sourcedir ui --target $@ --generate-header

$(GRESOURCES_SRC): $(GRESOURCE_BUNDLE) $(RESOURCES)
	glib-compile-resources $< --sourcedir ui --target $@ --generate-source


.PHONY: all release debug setup clean

build_mode_mismatch_debug0:
	@echo Specified build mode \(release\) does not match target build mode \(debug\)
	@test

build_mode_mismatch_debug1:
	@echo Specified build mode \(debug\) does not match target build mode \(release\)
	@test

clean:
	rm -rf $(GEN_DIRS)

-include $(wildcard $(DEPS))