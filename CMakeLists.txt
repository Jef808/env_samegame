cmake_minimum_required(VERSION 3.22)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SG_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_executable(main
  viewer.h
  viewer.cpp
  dsu.h
  dsu.cpp
  samegame.h
  samegame.cpp
  main.cpp)
target_compile_definitions(main PRIVATE "-DDATA_DIR=\"${SG_DATA_DIR}/\"")
