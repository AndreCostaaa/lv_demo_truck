cmake_minimum_required(VERSION 3.14)

project(
  lv_demo_truck
  VERSION 1.0.0
  LANGUAGES C CXX)

find_package(lvgl REQUIRED)

get_filename_component(REPO_ROOT
                       ${CMAKE_CURRENT_LIST_DIR}/../..
                       ABSOLUTE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${REPO_ROOT}/ui ui)

add_executable(lv_demo_truck ${REPO_ROOT}/src/main.c)
target_link_libraries(lv_demo_truck_ui PRIVATE lvgl::lvgl)
target_link_libraries(lv_demo_truck PRIVATE lvgl::lvgl)
