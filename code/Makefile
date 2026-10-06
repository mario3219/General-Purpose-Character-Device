ifneq ($(KERNELRELEASE),)

# ===========================================================================
# Kernel modules
# ===========================================================================

obj-m += stream_device.o
obj-m += stream_driver.o

# stream_driver.ko consists of multiple source files.
#
# stream_driver.c must be compiled under another object name because
# stream_driver.o is the final combined module object.
stream_driver-y := \
	stream_driver_main.o \
	src/stream_probe.o \
	src/stream_remove.o \
	src/stream_open.o \
	src/stream_release.o \
	src/stream_read.o \
	src/stream_write.o \
	src/stream_poll.o

# Allow:
#   #include "stream_driver.h"
ccflags-y += -I$(src)/include

# Compile stream_driver.c -> stream_driver_main.o
$(obj)/stream_driver_main.o: $(src)/stream_driver.c $(recordmcount_source) FORCE
	$(call if_changed_rule,cc_o_c)

else

KDIR ?= /path/to/linux
BUILD_DIR := $(CURDIR)/build

CXX := $(CROSS_COMPILE)g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic

.PHONY: all modules reader producer clean install

all: modules reader producer

modules:
	mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KDIR) \
		M=$(CURDIR) \
		MO=$(BUILD_DIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

reader: $(BUILD_DIR)/stream_reader
$(BUILD_DIR)/stream_reader: stream_reader.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

producer: $(BUILD_DIR)/stream_producer
$(BUILD_DIR)/stream_producer: stream_producer.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

install:
	install -D -m 0755 \
		$(BUILD_DIR)/stream_producer \
		$(DESTDIR)/usr/bin/stream_producer

	install -D -m 0644 \
		$(CURDIR)/systemd/stream_producer.service \
		$(DESTDIR)/usr/lib/systemd/system/stream_producer.service

clean:
	rm -rf $(BUILD_DIR)

endif
