obj-m += ecg.o

KDIR ?= /path/to/linux
BUILD_DIR := $(CURDIR)/build

CXX := $(CROSS_COMPILE)g++

all: module test

module:
	mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(KDIR) \
		M=$(CURDIR) \
		MO=$(BUILD_DIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

test: $(BUILD_DIR)/test

$(BUILD_DIR)/test: test.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) test.cc -o $(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all module test clean
