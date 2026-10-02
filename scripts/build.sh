#!/bin/bash

src_dir=$(realpath "$(pwd)/../")

cd ${src_dir}/code
./compile.sh

cp build/ecg.ko ${src_dir}/buildroot/output/target/
cp build/test ${src_dir}/buildroot/output/target/usr/bin/test_ecg

cd ${src_dir}/buildroot
make
