cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_C_COMPILER C:/MinGW/bin/gcc.exe)
set(CMAKE_CXX_COMPILER C:/MinGW/bin/g++.exe)
project(skylark)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_compile_definitions(SFML_STATIC)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)
file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(skylark WIN32 ${SRC})
target_link_libraries(skylark sfml-main sfml-graphics sfml-window sfml-audio sfml-system gdi32 winmm opengl32 openal32 vorbisfile vorbisenc vorbis ogg freetype FLAC)