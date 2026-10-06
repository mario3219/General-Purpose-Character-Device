#!/bin/bash

src_dir=$(realpath "$(pwd)/../")
target_dir="${src_dir}/buildroot/output/target"

cd ${src_dir}/code
./compile.sh

make install \
  DESTDIR="${target_dir}"

#cp build/stream_device.ko ${src_dir}/buildroot/output/target/
#cp build/stream_driver.ko ${src_dir}/buildroot/output/target/

mkdir -p ${target_dir}/lib/modules/6.18.7/extra
cp build/stream_device.ko ${target_dir}/lib/modules/6.18.7/extra/
cp build/stream_driver.ko ${target_dir}/lib/modules/6.18.7/extra/

cp build/stream_reader    ${target_dir}/usr/bin/
cp build/stream_producer  ${target_dir}/usr/bin/
cp ../data/data.raw       ${target_dir}/

cp ../configs/99-streamdev.rules ${target_dir}/etc/udev/rules.d/
cp ../configs/stream.conf        ${target_dir}/etc/modules-load.d/

cd ${src_dir}/buildroot
make
