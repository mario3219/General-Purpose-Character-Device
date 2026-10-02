#!/bin/bash

src_dir=$(realpath "$(pwd)/../")

cd ${src_dir}

if [ ! -d "${src_dir}/buildroot/.git" ]; then
  git clone https://gitlab.com/buildroot.org/buildroot.git
fi
