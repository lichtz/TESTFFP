# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6.0)

# Declares and names the project.
file(GLOB SOURCE *.cpp)
file(GLOB CSOURCE *.c)

include_directories(../ffmpeg/include)
include_directories(../ffmpeg/include/x264)
include_directories(../ffmpeg/include/fdk-aac)

add_library( # Sets the name of the library.
        livideoplayer
        SHARED
        ${SOURCE}
        ${CSOURCE}
        )




find_library( # Sets the name of the path variable.
        log-lib
        log)



target_link_libraries( # Specifies the target library.
        livideoplayer
        lichtmpeg
#        avfilter avformat avcodec avutil swresample swscale
        ${log-lib}
        android
        z
        OpenSLES
        m
        )



#add_library(avcodec
#          STATIC
#           IMPORTED
#        )
#
#set_target_properties(avcodec
#           PROPERTIES
#            IMPORTED_LOCATION
#            ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.a
#        )
#
#
#add_library(avcodec
#        SHARED
#        IMPORTED
#        )
#
#set_target_properties(avcodec
#        PROPERTIES
#        IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.so
#        )


