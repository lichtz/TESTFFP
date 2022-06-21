# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)
project("lipusher")
add_subdirectory(${CMAKE_SOURCE_DIR}/librtmp)


include_directories(${CMAKE_SOURCE_DIR}/x264/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI} ")

file(GLOB SOURCE *.cpp)

add_library( # Sets the name of the library.
        lipusher

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SOURCE})


find_library( # Sets the name of the path variable.
        log-lib

        log)


target_link_libraries(
        lipusher
        rtmp
        ${log-lib}
        x264
)