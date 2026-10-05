#!/bin/bash

src_dir=$(realpath "$(pwd)/../")

cd ${src_dir}/code
./compile.sh

cp build/stream_device.ko ${src_dir}/buildroot/output/target/
cp build/stream_driver.ko ${src_dir}/buildroot/output/target/
cp build/stream_reader ${src_dir}/buildroot/output/target/usr/bin/stream_reader

cd ${src_dir}/buildroot
make
