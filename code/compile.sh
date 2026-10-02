#!/bin/bash

src_dir=$(realpath "$(pwd)/../")
kernel_path=$(realpath "${src_dir}/buildroot/output/build/linux-6.18.7")
compiler_path=$(realpath "${src_dir}/buildroot/output/host/bin/aarch64-buildroot-linux-gnu-")

make \
  ARCH=arm64 \
  KDIR=${kernel_path} \
  CROSS_COMPILE=${compiler_path}
