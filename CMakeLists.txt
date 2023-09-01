cmake_minimum_required(VERSION 3.26)
project(XaerotoJourneyMap)

add_custom_target(
        StateToColorHeader ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/dummyHeaderBlehh.h
)

add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_SOURCE_DIR}/resources/dummyHeaderBlehh.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/lookup/stateToColor.h
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/resources/build.sh
)

set(CMAKE_CXX_STANDARD 23)

add_executable(XaerotoJourneyMap src/main.cpp src/converter.cpp src/converter.h src/byteParser.cpp src/colorConverter.cpp src/lookup/stateToColor.h)

target_include_directories(XaerotoJourneyMap PUBLIC /usr/local/include/Poco)
target_include_directories(XaerotoJourneyMap PUBLIC /usr/include/stb)

target_link_libraries(XaerotoJourneyMap PocoZip PocoFoundation)

target_link_directories(XaerotoJourneyMap PUBLIC /usr/local/lib)