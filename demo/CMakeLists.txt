cmake_minimum_required(VERSION 3.16)
project(lvgl_offscreen C CXX)

set(CMAKE_C_STANDARD 99)

# LVGL 源码目录，默认取仓库内 third_party/lvgl
if(NOT DEFINED LVGL_DIR)
  set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lvgl")
endif()

set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE STRING "" FORCE)

add_subdirectory(${LVGL_DIR} lvgl)

if(MSVC)
  add_compile_options(/utf-8)
endif()

add_executable(render main.c)
target_compile_definitions(render PRIVATE LV_CONF_INCLUDE_SIMPLE)
if(WIN32)
  target_link_libraries(render PRIVATE lvgl)
else()
  target_link_libraries(render PRIVATE lvgl m)
endif()
