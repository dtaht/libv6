#!/bin/sh
# Build for everything I got

export STAGING_DIR=/home/d/git/lede-sdk-17.01.0-ramips-mt7621_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/

export PATH=/home/d/git/lede-sdk-17.01.0-ramips-mt7621_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin/:$PATH

COMPILERS="gcc aarch64-linux-gnu-gcc arm-linux-gnueabi-gcc mipsel-openwrt-linux-gcc"

for i in $COMPILERS
do
	mkdir -p build-$i 2> /dev/null
	make CC=$i TARGET=build-$i
done
