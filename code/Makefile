ifneq ($(KERNELRELEASE),)

# ===========================================================================
# Kernel build
# ===========================================================================

obj-m += stream_driver.o
obj-m += stream_device.o

# stream_driver.ko is composed of these source files.
stream_driver-y := \
	stream_driver_main.o \
	src/stream_probe.o \
	src/stream_remove.o \
	src/stream_open.o \
	src/stream_release.o \
	src/stream_read.o \
	src/stream_write.o \
	src/stream_poll.o \
	src/stream_timer_callback.o

# Allow source files to include:
#
#   #include "stream_driver.h"
#
ccflags-y += -I$(src)/include

# stream_driver.c is the entry point for stream_driver.ko.
#
# We compile it as stream_driver_main.o because stream_driver.o is already
# used by Kbuild as the combined object for the complete module.
$(obj)/stream_driver_main.o: $(src)/stream_driver.c $(recordmcount_source) FORCE
	$(call if_changed_rule,cc_o_c)

else

# ===========================================================================
# External build
# ===========================================================================

KDIR ?= /lib/modules/$(shell uname -r)/build

BUILD_DIR := $(CURDIR)/build

CXX := $(CROSS_COMPILE)g++

KBUILD_ARGS := \
	$(if $(ARCH),ARCH=$(ARCH)) \
	$(if $(CROSS_COMPILE),CROSS_COMPILE=$(CROSS_COMPILE))

.DEFAULT_GOAL := all

# ===========================================================================
# Main targets
# ===========================================================================

all: modules userspace

# ---------------------------------------------------------------------------
# Kernel modules
# ---------------------------------------------------------------------------

modules:
	mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KDIR) \
		M=$(CURDIR) \
		MO=$(BUILD_DIR) \
		$(KBUILD_ARGS) \
		modules

# Alias if you still want to use "make module".
module: modules

# ---------------------------------------------------------------------------
# Userspace programs
# ---------------------------------------------------------------------------

userspace: \
	$(BUILD_DIR)/stream_reader \
	$(BUILD_DIR)/stream_producer

# Keep your previous "make test" interface as an alias.
test: userspace

$(BUILD_DIR)/stream_reader: stream_reader.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		$< \
		$(LDFLAGS) \
		$(LDLIBS) \
		-o $@

$(BUILD_DIR)/stream_producer: stream_producer.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		$< \
		$(LDFLAGS) \
		$(LDLIBS) \
		-o $@

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all modules module userspace test clean

endif
