cmake_minimum_required(VERSION 3.20)
project(XaerotoJourneyMap)
set(CMAKE_CXX_STANDARD 23)
add_executable(XaerotoJourneyMap src/main.cpp src/converter.cpp src/converter.h src/byteParser.cpp src/colorConverter.cpp src/lookup/stateToColor.h)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/lookup/stateToColor.h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/resources
        COMMAND cmake . && cmake --build . && ./Bundler blockstateidtocolor.txt ../src/lookup/stateToColor.h
)

file(DOWNLOAD
        https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
        ${CMAKE_SOURCE_DIR}/include/stb_image_write.h
)

file (DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake
        ${CMAKE_SOURCE_DIR}/include/cmake/get_cpm.cmake
)

file(DOWNLOAD
        https://raw.githubusercontent.com/DanDucky/ArgumentParser/master/include/CLIParser.hpp
        ${CMAKE_SOURCE_DIR}/include/CLIParser.hpp
)

include(include/cmake/get_cpm.cmake)

CPMAddPackage(
        NAME libzip
        GIT_TAG v1.10.1
        GITHUB_REPOSITORY nih-at/libzip
        OPTIONS
            "BUILD_SHARED_LIBS OFF"
            "ENABLE_BZIP2 OFF"
            "ENABLE_LZMA OFF"
            "ENABLE_ZSTD OFF"
            "ENABLE_COMMONCRYPTO OFF"
            "LIBZIP_DO_INSTALL ON"
)
CPMAddPackage(
        NAME libzippp
        GIT_TAG libzippp-v7.1-1.10.1
        GITHUB_REPOSITORY ctabin/libzippp
        OPTIONS
            "CMAKE_BUILD_TYPE Release"
            "BUILD_SHARED_LIBS OFF"
)

configure_file (
        ${libzippp_SOURCE_DIR}/src/libzippp.h
        ${CMAKE_SOURCE_DIR}/include
        COPYONLY
)

target_include_directories(XaerotoJourneyMap PUBLIC include)

target_link_libraries(XaerotoJourneyMap stdc++ m libzippp)

install(TARGETS XaerotoJourneyMap)