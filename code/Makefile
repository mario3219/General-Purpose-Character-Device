ifneq ($(KERNELRELEASE),)
obj-m += stream_driver.o stream_device.o
stream_driver-y := stream_driver_main.o \
	src/stream_probe.o \
	src/stream_remove.o \
	src/stream_open.o \
	src/stream_release.o \
	src/stream_read.o \
	src/stream_poll.o \
	src/stream_timer_callback.o

# Keep the entry-point object distinct from the combined module object.
$(obj)/stream_driver_main.o: $(src)/stream_driver.c $(recordmcount_source) FORCE
	$(call if_changed_rule,cc_o_c)
else

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(CURDIR)/build

CXX := $(CROSS_COMPILE)g++
KBUILD_ARGS := $(if $(ARCH),ARCH=$(ARCH)) \
	$(if $(CROSS_COMPILE),CROSS_COMPILE=$(CROSS_COMPILE))

.DEFAULT_GOAL := all

all: module test

module:
	mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KDIR) \
		M=$(CURDIR) \
		MO=$(BUILD_DIR) \
		$(KBUILD_ARGS) \
		modules

test: $(BUILD_DIR)/stream_reader

$(BUILD_DIR)/stream_reader: stream_reader.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all module test clean
endif
