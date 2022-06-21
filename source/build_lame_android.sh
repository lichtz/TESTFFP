#!/bin/bash
NDK=/root/ffmpeg/android-ndk-r22b # 这里需要替换成你本地的 NDK 路径，其他的不用修改
HOST_TAG=linux-x86_64
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/$HOST_TAG
ANDROID_LIB_PATH="$(pwd)/android"



API=21

function build_android
{
echo "Build lame for android $CPU"
./configure \
--host=$HOST \
--disable-shared \
--disable-frontend \
--enable-static \
--prefix="$ANDROID_LIB_PATH/$CPU" \

make clean
make -j8
make install
echo "Build lame for android $CPU completed"
}

# armeabi-v7a
CPU=armv7-a
HOST=arm-linux-android
export AR=$TOOLCHAIN/bin/arm-linux-androideabi-ar
export AS=$TOOLCHAIN/bin/arm-linux-androideabi-as
export LD=$TOOLCHAIN/bin/arm-linux-androideabi-ld
export RANLIB=$TOOLCHAIN/bin/arm-linux-androideabi-ranlib
export STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip
export CC=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang
export CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang++

# PREBUILT=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
# PLATFORM=$TOOLCHAIN/sysroot
# export PATH=$PATH:$PREBUILT/bin:$PLATFORM/usr/include
# export LDFLAGS="-L$PLATFORM/usr/lib -L$PREBUILT/arm-linux-androideabi/lib -march=armv7-a"
# export CFLAGS="-I$PLATFORM/usr/include -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -ffast-math -O2"
# -mfloat-abi=softfp -mfpu=vfp 可能需要gcc内容
export LDFLAGS="-march=$CPU"
export CFLAGS="-march=$CPU -mfloat-abi=softfp -mfpu=vfp -ffast-math -O2"
build_android

# arm64-v8a
CPU=armv8-a
HOST=aarch64-linux-android
export AR=$TOOLCHAIN/bin/aarch64-linux-android-ar
export AS=$TOOLCHAIN/bin/aarch64-linux-android-as
export LD=$TOOLCHAIN/bin/aarch64-linux-android-ld
export RANLIB=$TOOLCHAIN/bin/aarch64-linux-android-ranlib
export STRIP=$TOOLCHAIN/bin/aarch64-linux-android-strip
export CC=$TOOLCHAIN/bin/aarch64-linux-android$API-clang
export CXX=$TOOLCHAIN/bin/aarch64-linux-android$API-clang++
export LDFLAGS="-march=$CPU"
export CFLAGS="-march=$CPU -mfloat-abi=softfp -mfpu=vfp -ffast-math -O2"
build_android