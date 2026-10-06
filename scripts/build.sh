#!/bin/bash

src_dir=$(realpath "$(pwd)/../")
target_dir="${src_dir}/buildroot/output/target"

cd ${src_dir}/code
./compile.sh

# Installs the producer service
make install \
  DESTDIR="${target_dir}"

# Install device and driver
mkdir -p ${target_dir}/lib/modules/6.18.7/extra
cp build/stream_device.ko ${target_dir}/lib/modules/6.18.7/extra/
cp build/stream_driver.ko ${target_dir}/lib/modules/6.18.7/extra/

# Reader and producer executibles
cp build/stream_reader    ${target_dir}/usr/bin/
cp build/stream_producer  ${target_dir}/usr/bin/
cp ../data/data.raw       ${target_dir}/

# Device user permissions
cp ../configs/99-streamdev.rules ${target_dir}/etc/udev/rules.d/

# Device and driver automatic load config
cp ../configs/stream.conf        ${target_dir}/etc/modules-load.d/

cd ${src_dir}/buildroot
make
