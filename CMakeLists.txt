cmake_minimum_required(VERSION 3.14)
project(
  lv_demo_truck
  VERSION 1.0.0
  LANGUAGES C CXX)

# Allow including `.cmake` files inside the `cmake` folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(dependencies)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ui)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
add_executable(lv_demo_truck ${SOURCES} ${ASSETS})

target_compile_features(lv_demo_truck PUBLIC c_std_99)

if(CONFIG_LV_USE_GLTF)
  message(STATUS "Compiling with GLTF support")
  include(FetchContent)

  FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf
    GIT_TAG f04052ebd8d157c7b6e8fc3dd9f1ed83df99f3e5)

  set(FASTGLTF_ENABLE_DEPRECATED_EXT
      ON
      CACHE BOOL "" FORCE)

  set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
      ON
      CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(fastgltf)

  set(WEBP_BUILD_ANIM_UTILS
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_CWEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_DWEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_GIF2WEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_IMG2WEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_VWEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_WEBPINFO
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_WEBPMUX
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_EXTRAS
      OFF
      CACHE BOOL "" FORCE)

  FetchContent_Declare(
    webp
    GIT_REPOSITORY https://github.com/webmproject/libwebp
    GIT_TAG fa6f56496a442eed59b103250021e4b14ebf1427)

  FetchContent_MakeAvailable(webp)
  target_link_libraries(lvgl PUBLIC fastgltf webp)

endif()

find_package(PkgConfig REQUIRED)
if(CONFIG_LV_USE_EVDEV)
  message(STATUS "Compiling with EVDEV support")
  pkg_check_modules(EVDEV REQUIRED libevdev)
  target_link_libraries(lvgl PUBLIC ${EVDEV_LIBRARIES})
  target_include_directories(lvgl PUBLIC ${EVDEV_INCLUDE_DIRS})
endif()

if(CONFIG_LV_USE_WAYLAND)
  message(STATUS "Compiling with Wayland support")

  pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
  pkg_check_modules(WAYLAND_CURSOR REQUIRED wayland-cursor)
  pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)

  target_link_libraries(
    lvgl PUBLIC ${WAYLAND_CURSOR_LIBRARIES} ${XKBCOMMON_LIBRARIES}
                ${WAYLAND_CLIENT_LIBRARIES} ${WAYLAND_CURSOR_LIBRARIES})

  # Wayland protocols
  pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.25)
  pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)

  if(DEFINED ENV{SDKTARGETSYSROOT} AND DEFINED ENV{SYSROOT})
    message(
      FATAL_ERROR
        "Both SDKTARGETSYSROOT and SYSROOT are set. Please set only one.")
  endif()

  if(DEFINED ENV{SDKTARGETSYSROOT})
    set(PROTOCOL_ROOT "$ENV{SDKTARGETSYSROOT}/usr/share/wayland-protocols")
  elseif(DEFINED ENV{SYSROOT})
    set(PROTOCOL_ROOT "$ENV{SYSROOT}/usr/share/wayland-protocols")
  else()
    set(PROTOCOL_ROOT "/usr/share/wayland-protocols")
  endif()

  if(NOT EXISTS ${PROTOCOL_ROOT})
    message(FATAL_ERROR "Wayland protocols not found at ${PROTOCOL_ROOT}")
  endif()

  set(PROTOCOLS_DIR "${CMAKE_CURRENT_BINARY_DIR}/protocols")
  file(MAKE_DIRECTORY ${PROTOCOLS_DIR})

  set(WAYLAND_PROTOCOLS_SRC "")

  # Use public-code for shared libs, private-code for static libs
  if(BUILD_SHARED_LIBS)
    set(WAYLAND_SCANNER_CODE_MODE "public-code")
  else()
    set(WAYLAND_SCANNER_CODE_MODE "private-code")
  endif()

  # Generate xdg-shell protocol (always)
  set(XDG_SHELL_XML "${PROTOCOL_ROOT}/stable/xdg-shell/xdg-shell.xml")
  set(XDG_SHELL_HEADER "${PROTOCOLS_DIR}/wayland_xdg_shell.h")
  set(XDG_SHELL_SOURCE "${PROTOCOLS_DIR}/wayland_xdg_shell.c")

  if(NOT EXISTS ${XDG_SHELL_HEADER} OR NOT EXISTS ${XDG_SHELL_SOURCE})
    execute_process(COMMAND wayland-scanner client-header ${XDG_SHELL_XML}
                            ${XDG_SHELL_HEADER})
    execute_process(COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE}
                            ${XDG_SHELL_XML} ${XDG_SHELL_SOURCE})
  endif()

  list(APPEND WAYLAND_PROTOCOLS_SRC ${XDG_SHELL_SOURCE})

  if(CONFIG_LV_WAYLAND_USE_EGL)
    message(STATUS "Compiling with Wayland EGL support")
    pkg_check_modules(WAYLAND_EGL wayland-egl)

    target_link_libraries(lvgl PUBLIC ${WAYLAND_EGL_LIBRARIES})
    target_include_directories(lvgl PUBLIC ${WAYLAND_EGL_INCLUDE_DIRS})
  endif()

  add_library(lv_demo_truck_wayland_protocols ${WAYLAND_PROTOCOLS_SRC})
  target_include_directories(lvgl PUBLIC ${PROTOCOLS_DIR})
endif()

if(CONFIG_LV_USE_SDL)
  message(STATUS "Compiling with SDL support")
  pkg_check_modules(SDL2 REQUIRED sdl2)

  target_link_libraries(lv_demo_truck PUBLIC ${SDL2_LIBRARIES})
  target_include_directories(lv_demo_truck PUBLIC ${SDL2_INCLUDE_DIRS})
endif()

if(CONFIG_LV_USE_GLFw)
  message(STATUS "Compiling with GLFW support")
  pkg_check_modules(GLFW3 REQUIRED glfw3)

  target_link_libraries(lv_demo_truck PUBLIC ${GLFW3_LIBRARIES})
  target_include_directories(lv_demo_truck PUBLIC ${GLFW3_INCLUDE_DIRS})
endif()

if(CONFIG_LV_USE_LINUX_DRM)
  message(STATUS "Compiling with DRM support")
  pkg_check_modules(LIBDRM REQUIRED libdrm)
  pkg_check_modules(GBM REQUIRED gbm)

  target_link_libraries(lv_demo_truck PUBLIC ${GBM_LIBRARIES}
                                             ${LIBDRM_LIBRARIES})
  target_include_directories(lv_demo_truck PUBLIC ${GBM_INCLUDE_DIRS}
                                                  ${LIBDRM_INCLUDE_DIRS})

  target_link_libraries(lvgl PUBLIC ${GBM_LIBRARIES} ${LIBDRM_LIBRARIES})
  target_include_directories(lvgl PUBLIC ${GBM_INCLUDE_DIRS}
                                         ${LIBDRM_INCLUDE_DIRS})
endif()

if(ASAN)
  message(STATUS "Enabling Sanitizer")
  target_compile_options(lv_demo_truck PUBLIC -fsanitize=address,undefined,null)
  target_link_options(lv_demo_truck PUBLIC
                      -fsanitize=address,undefined,null,leak)
  target_compile_options(lvgl PUBLIC -fsanitize=address,undefined,null)
  target_link_options(lvgl PUBLIC -fsanitize=address,undefined,null,leak)
endif()

if(CONFIG_LV_USE_WAYLAND)
  target_link_libraries(lv_demo_truck PUBLIC lv_demo_truck_ui lvgl
                                             lv_demo_truck_wayland_protocols)
else()
  target_link_libraries(lv_demo_truck PUBLIC lv_demo_truck_ui lvgl)
endif()
