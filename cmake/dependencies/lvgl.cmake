set(LV_CONF_DEFAULTS_PATH "${PROJECT_SOURCE_DIR}/lv_conf.defaults")

set(LV_BUILD_CONF_PATH
    "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "" FORCE)

set(LVGL_TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/lvgl/lv_conf_template.h")
set(GENERATE_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/lvgl/scripts/generate_lv_conf.py")

message(
  STATUS
    "Using LVGL from ${PROJECT_SOURCE_DIR}/lvgl with ${LV_CONF_DEFAULTS_PATH} config"
)

set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS ${LVGL_TEMPLATE_PATH})
set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS ${LV_CONF_DEFAULTS_PATH})
set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS ${GENERATE_SCRIPT_PATH})

execute_process(
  COMMAND
    ${Python3_EXECUTABLE} ${GENERATE_SCRIPT_PATH} --template
    ${LVGL_TEMPLATE_PATH} --defaults ${LV_CONF_DEFAULTS_PATH} --config
    ${LV_BUILD_CONF_PATH}
  RESULT_VARIABLE config_result
  OUTPUT_VARIABLE config_output
  ERROR_VARIABLE config_output)

if(NOT config_result EQUAL 0)
  message(FATAL_ERROR "Failed to generate lv_conf.h: ${config_output}")
endif()

message(STATUS "${LV_BUILD_CONF_PATH} generated successfully")

set(CONFIG_LV_BUILD_EXAMPLES
    OFF
    CACHE BOOL "disable lvgl examples" FORCE)

set(CONFIG_LV_USE_THORVG_INTERNAL
    OFF
    CACHE BOOL "disable thorvg internal" FORCE)

set(CONFIG_LV_BUILD_DEMOS
    OFF
    CACHE BOOL "disable lvgl demos" FORCE)

set(LV_BUILD_SET_CONFIG_OPTS
    ON
    CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/lvgl ${CMAKE_BINARY_DIR}/lvgl)

if(NOT WIN32)
  cmake_policy(SET CMP0079 NEW)
  target_link_libraries(lvgl PUBLIC m)
endif()
