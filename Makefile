CC                = gcc
BUILD_DIR         = build
BIN_DIR           = bin
NAME              = main
TARGET            = $(BIN_DIR)/$(NAME)

VPATH             = src src/device src/hotplug src/templates

UI_FILES         := ui/gresources.gresource.xml $(shell find ui -name *.ui -o -name *.css)
GRESOURCES_SRC    = resources/gresources.c
GRESOURCES_HEADER = resources/gresources.h
GRESOURCES_OBJ    = $(BUILD_DIR)/gresources.o

ifeq ($(debug), 1)
	DEBUG_FLAGS = -fsanitize=address -g -Og
else
	debug = 0
endif

# a bit of sanity checking for the build mode
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
OBJS    := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS)) 
DEPDIR   = $(BUILD_DIR)/.deps
DEPFLAGS = -MT $@ -MM -MP -MF $(DEPDIR)/$*.d

# The find command in SRCS will error if resources/ does not exist
VPATH += resources

GEN_DIRS = resources $(DEPDIR) $(BUILD_DIR) $(BIN_DIR)

all: $(ACTUAL_TARGET)
	
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

.PHONY: all clean build_mode_mismatch_debug build_mode_mismatch_release

build_mode_mismatch_release:
	@echo Specified build mode \(release\) does not match target build mode \(debug\)
	@test

build_mode_mismatch_debug:
	@echo Specified build mode \(debug\) does not match target build mode \(release\)
	@test

clean:
	rm -rf $(GEN_DIRS)

