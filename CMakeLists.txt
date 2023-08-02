cmake_minimum_required(VERSION 3.26)
project(XaerotoJourneyMap)

set(CMAKE_CXX_STANDARD 23)

add_executable(XaerotoJourneyMap src/main.cpp src/converter.cpp src/converter.h src/byteParser.cpp)

target_include_directories(XaerotoJourneyMap PUBLIC /usr/local/include/Poco)
target_include_directories(XaerotoJourneyMap PUBLIC /usr/include/stb)

target_link_libraries(XaerotoJourneyMap PocoZip PocoFoundation)

target_link_directories(XaerotoJourneyMap PUBLIC /usr/local/lib)