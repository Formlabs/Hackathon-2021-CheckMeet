cmake_minimum_required (VERSION 3.16)

project (firmware LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

add_library (lib_firmware INTERFACE)
target_sources (lib_firmware
    INTERFACE
        stdextra.h
        lib_firmware.h
        ArduinoJson-v6.18.0.h
)
target_include_directories (lib_firmware INTERFACE "${CMAKE_CURRENT_SOURCE_DIRECTORY}")

add_executable (catch_firmware
    catch/catch.hpp
    catch/catch_firmware.cpp
)

target_link_libraries (catch_firmware
    PRIVATE
        lib_firmware
)
