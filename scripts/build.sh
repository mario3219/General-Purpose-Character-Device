#!/bin/bash

src_dir=$(realpath "$(pwd)/../")

cd ${src_dir}/code
./compile.sh

cp build/stream_device.ko ${src_dir}/buildroot/output/target/
cp build/stream_driver.ko ${src_dir}/buildroot/output/target/
cp build/stream_reader    ${src_dir}/buildroot/output/target/usr/bin/
cp build/stream_producer  ${src_dir}/buildroot/output/target/usr/bin/
cp ../data/data.raw       ${src_dir}/buildroot/output/target/

cd ${src_dir}/buildroot
make
