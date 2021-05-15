cmake_minimum_required (VERSION 3.16)

project (firmware LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable most warnings (https://cmake.org/cmake/help/latest/command/add_compile_options.html)
if (MSVC)
    add_compile_options(/W3 /WX)
    add_compile_options(/wd4996) # see https://bugreports.qt.io/browse/QTBUG-84661 & commit message
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

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
