cmake_minimum_required(VERSION 3.20)
project(CxxReflectionProject)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# ----------------------------
# Subdirectories
# ----------------------------
add_subdirectory(ReflectionTemplateLib)
add_subdirectory(rtl-demo)
