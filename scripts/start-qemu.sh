#!/bin/bash

src_dir=$(realpath "$(pwd)/../")

cd ${src_dir}/buildroot/

qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a53 \
    -m 512M \
    -nographic \
    -kernel output/images/Image \
    -append "console=ttyAMA0 root=/dev/vda rw" \
    -drive file=output/images/rootfs.ext4,if=virtio,format=raw
