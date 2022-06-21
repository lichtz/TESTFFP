# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html


cmake_minimum_required(VERSION 3.6.0)

file(GLOB SOURCE *.cpp)
file(GLOB CSOURCE *.c)
#file(GLOB CSOURCE audio/*.cpp)
#project("openglesExx")

include_directories(../ffmpeg/include)
include_directories(include)
#include_directories(audio/*.h)

add_library( # Sets the name of the library.
        openglesExx
        SHARED
        ${SOURCE}
#        ${CSOURCE}
        )




find_library( # Sets the name of the path variable.
        log-lib
        log)
target_link_libraries( # Specifies the target library.
        openglesExx
        livideoplayer
        ${log-lib}
        android
        EGL
        GLESv3
        OpenSLES
        )






