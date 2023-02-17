# Минимальная версия CMake
cmake_minimum_required(VERSION 3.0)

project(RTS VERSION 0.1 LANGUAGES CXX)

set(NAME "Matveev")
set(GROUP "8362")

set(REPORTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/reports/")


add_subdirectory(lab1)
add_subdirectory(lab2)
