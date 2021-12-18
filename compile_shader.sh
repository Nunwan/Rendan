#!/usr/bin/bash

BUILDDIR=../build/shaders
mkdir -p ./build/shaders;
cd shaders || exit 1;
for shader in *; do
glslc ${shader} -o ${BUILDDIR}/${shader}.spv
done;

cd .. || exit 1;


