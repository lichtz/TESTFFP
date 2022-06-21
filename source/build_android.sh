#!/bin/bash
NDK=/root/ffmpeg/android-ndk-r22b
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64
API=21

function build_android
{

./configure \
--prefix=$PREFIX \
--enable-neon  \
--enable-hwaccels  \
--enable-gpl   \
--disable-postproc \
--enable-static \
--disable-shared \
--disable-debug \
--enable-small \
--enable-jni \
--enable-mediacodec \
--disable-doc \
--enable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-avdevice \
--disable-doc \
--disable-symver \
--enable-libx264 \
--enable-libfdk-aac \
--enable-encoder=libx264 \
--enable-encoder=libfdk-aac \
--enable-nonfree \
--enable-muxers \
--enable-decoders \
--enable-demuxers \
--enable-parsers \
--enable-protocols \
--cross-prefix=$CROSS_PREFIX \
--target-os=android \
--arch=$ARCH \
--cpu=$CPU \
--cc=$CC \
--cxx=$CXX \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags="-I$X264_INCLUDE -I$FDK_INCLUDE -Os -fpic $OPTIMIZE_CFLAGS" \
--extra-ldflags="-lm -L$FDK_LIB -L$X264_LIB $ADDI_LDFLAGS" 

make clean
make -j8
make install
echo "The Compilation of FFmpeg with x264,fdk-aac for $CPU is completed"
}

#armv8-a
ARCH=arm64
CPU=armv8-a
API=21
CC=$TOOLCHAIN/bin/aarch64-linux-android$API-clang
CXX=$TOOLCHAIN/bin/aarch64-linux-android$API-clang++
SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android-
PREFIX=$(pwd)/android/armv8-a
OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "

# 头文件目录
FDK_INCLUDE=/root/ffmpeg/ffmpeg-4.4/thiredSource/fdk_aac/armv8-a/include
# 库文件目录
FDK_LIB=/root/ffmpeg/ffmpeg-4.4/thiredSource/fdk_aac/armv8-a/lib
X264_INCLUDE=/root/ffmpeg/ffmpeg-4.4/thiredSource/x264/armv8-a/include
X264_LIB=/root/ffmpeg/ffmpeg-4.4/thiredSource/x264/armv8-a/lib

build_android

cp $FDK_LIB/libfdk-aac.a $PREFIX/lib
cp $X264_LIB/libx264.a $PREFIX/lib

#armv7-a
ARCH=arm
CPU=armv7-a
API=21
CC=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang
CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang++
SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi-
PREFIX=$(pwd)/android/armv7-a
OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "
# 头文件目录
FDK_INCLUDE=/root/ffmpeg/ffmpeg-4.4/thiredSource/fdk_aac/armv7-a/include
# 库文件目录
FDK_LIB=/root/ffmpeg/ffmpeg-4.4/thiredSource/fdk_aac/armv7-a/lib
X264_INCLUDE=/root/ffmpeg/ffmpeg-4.4/thiredSource/x264/armv7-a/include
X264_LIB=/root/ffmpeg/ffmpeg-4.4/thiredSource/x264/armv7-a/lib
build_android

cp $FDK_LIB/libfdk-aac.a $PREFIX/lib
cp $X264_LIB/libx264.a $PREFIX/lib






















# #armv8-a
# ARCH=arm64
# CPU=armv8-a
# # 头文件目录
# FDK_INCLUDE=$LIBS_DIR/fdk_aac/$CPU/include
# # 库文件目录
# FDK_LIB=$LIBS_DIR/fdk_aac/$CPU/lib
# X264_INCLUDE=$LIBS_DIR/x264/$CPU/include
# X264_LIB=$LIBS_DIR/x264/$CPU/lib
# # OPENSSL_INCLUDE=$LIBS_DIR/openssl/$CPU/include
# # OPENSSL_LIB=$LIBS_DIR/openssl/$CPU/lib


# CC=$TOOLCHAIN/bin/aarch64-linux-android$API-clang
# CXX=$TOOLCHAIN/bin/aarch64-linux-android$API-clang++
# SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
# CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android-
# PREFIX=$(pwd)/android/$CPU
# OPTIMIZE_CFLAGS="-march=$CPU"
# build_android
 
# #armv7-a
# ARCH=armeabi-v7a
# CPU=armv7-a


# # 头文件目录
# FDK_INCLUDE=$LIBS_DIR/fdk_aac/$CPU/include
# # 库文件目录
# FDK_LIB=$LIBS_DIR/fdk_aac/$CPU/lib
# X264_INCLUDE=$LIBS_DIR/x264/$CPU/include
# X264_LIB=$LIBS_DIR/x264/$CPU/lib
# # OPENSSL_INCLUDE=$LIBS_DIR/openssl/$CPU/include
# # OPENSSL_LIB=$LIBS_DIR/openssl/$CPU/lib


# CC=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang
# CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang++
# SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
# CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi-
# PREFIX=$(pwd)/android/$CPU
# build_android
 
# #x86
# ARCH=x86
# CPU=x86
# CC=$TOOLCHAIN/bin/i686-linux-android$API-clang
# CXX=$TOOLCHAIN/bin/i686-linux-android$API-clang++
# SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
# CROSS_PREFIX=$TOOLCHAIN/bin/i686-linux-android-
# PREFIX=$(pwd)/android/$CPU
# build_android
 
# #x86_64
# ARCH=x86_64
# CPU=x86-64
# CC=$TOOLCHAIN/bin/x86_64-linux-android$API-clang
# CXX=$TOOLCHAIN/bin/x86_64-linux-android$API-clang++
# SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
# CROSS_PREFIX=$TOOLCHAIN/bin/x86_64-linux-android-
# PREFIX=$(pwd)/android/$CPU
# build_android

