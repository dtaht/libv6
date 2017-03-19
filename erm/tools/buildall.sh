#!/bin/sh

# Do a build for all the compilers I have handy
# openwrt is weird for me

export STAGING_DIR=/home/d/git/lede-sdk-17.01.0-ramips-mt7621_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/

export PATH=/home/d/git/lede-sdk-17.01.0-ramips-mt7621_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin/:$PATH


ICC=`which icc`
GCC=`which gcc`
AARCH64=`which aarch64-linux-gnu-gcc-5`
ARM=`which arm-linux-gnueabihf-gcc`
MIPS=`which mipsel-openwrt-linux-gcc`
PARALLELLA=`which e-gcc`

for i in $ICC $GCC $AARCH64 $ARM $PARALLELLA $MIPS
do
b=`basename $i`
mkdir -p $b
echo building: $b
make CC=$b TARGET=$b
done

