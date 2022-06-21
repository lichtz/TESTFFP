# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html
cmake_minimum_required(VERSION 3.6.0)
message(WANNING "******************************************************************")
message(WANNING "CMAKE_SOURCE_DIR->" ${CMAKE_SOURCE_DIR})
message(WANNING "PROJECT_SOURCE_DIR->)" ${PROJECT_SOURCE_DIR})
message(WANNING "******************************************************************")


project("OneSDK")
set(commonLibPath  "${CMAKE_SOURCE_DIR}/source/ffmpeg/libs/${ANDROID_ABI}")
message(WANNING ${commonLibPath})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${commonLibPath}")
add_subdirectory(source/ffmpeg)
add_subdirectory(source/player)
add_subdirectory(source/opengles)











