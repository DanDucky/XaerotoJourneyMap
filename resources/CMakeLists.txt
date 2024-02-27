cmake_minimum_required(VERSION 3.20)
project(Bundler)

set(CMAKE_CXX_STANDARD 23)

add_executable(Bundler main.cpp)

set(CMAKE_CXX_FLAGS -static)

target_link_libraries(Bundler stdc++)
