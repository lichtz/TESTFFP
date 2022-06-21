# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DNO_CRYPTO")
add_library(
        rtmp
        STATIC
        amf.c
        hashswf.c
        log.c
        parseurl.c
        rtmp.c
)







