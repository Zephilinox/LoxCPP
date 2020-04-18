cmake_minimum_required (VERSION 3.15)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project ("LoxCPPSolution")

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/MP>")

add_subdirectory ("src")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT LoxCPP)

#add_subdirectory ("third_party" EXCLUDE_FROM_ALL)