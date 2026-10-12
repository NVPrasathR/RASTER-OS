# PACSCORDER — top-level build (ADR-014: CMake + pkg-config + CTest).
#
#   cmake -S . -B build
#   cmake --build build
#   ctest --test-dir build --output-on-failure
#
# See docs/BUILD_SYSTEM.md.
cmake_minimum_required(VERSION 3.20)
project(pacscorder VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
# 1.26 is the GStreamer version Raspberry Pi OS ships ([G-26], [G-27]).
pkg_check_modules(GST REQUIRED IMPORTED_TARGET
  gstreamer-1.0>=1.26 gstreamer-app-1.0>=1.26 gstreamer-video-1.0>=1.26 glib-2.0)

add_subdirectory(app)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
