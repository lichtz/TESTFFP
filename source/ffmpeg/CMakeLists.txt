# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6.0)
include_directories(include)
add_library(
        lichtmpeg
        SHARED
        fmain.cpp
)

#set_target_properties(
#        lichtFFmpeg
#        PROPERTIES
#        IMPORTED_LOCATION
#        libs/${CMAKE_ANDROID_ARCH_ABI}/libavfilter.a
#)
#set_target_properties(
#        lichtFFmpeg
#        PROPERTIES
#        IMPORTED_LOCATION
#        libs/${CMAKE_ANDROID_ARCH_ABI}/libavformat.a
#)
#
#set_target_properties(
#        lichtFFmpeg
#        PROPERTIES
#        IMPORTED_LOCATION
#        libs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.a
#
#)
#
#
#set_target_properties(
#        lichtFFmpeg
#        PROPERTIES
#        IMPORTED_LOCATION
#        libs/${CMAKE_ANDROID_ARCH_ABI}/libavutil.a
#)
#set_target_properties(
#        lichtFFmpeg
#        PROPERTIES
#        IMPORTED_LOCATION
#        libs/${CMAKE_ANDROID_ARCH_ABI}/libswresample.a
#)
#
#set_target_properties(
#        lichtFFmpeg
#        PROPERTIES
#        IMPORTED_LOCATION
#        libs/${CMAKE_ANDROID_ARCH_ABI}/libswscale.a
#)
#
#



target_link_libraries( # Specifies the target library.
        lichtmpeg
        avfilter avformat avcodec avutil swresample swscale
        fdk-aac
        x264
        )
