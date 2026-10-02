#!/bin/bash

src_dir=$(realpath "$(pwd)/../")

cd ${src_dir}

if [ ! -d "${src_dir}/buildroot/.git" ]; then
  git clone https://gitlab.com/buildroot.org/buildroot.git
fi

cp -r "${src_dir}/configs/buildroot_config" "${src_dir}/buildroot/configs/custom_defconfig"
cd buildroot
make custom_defconfig
