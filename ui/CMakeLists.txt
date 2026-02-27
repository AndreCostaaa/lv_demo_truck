set(SOURCES src/lv_demo_truck.c)
add_library(lv_demo_truck_ui ${SOURCES})

target_compile_definitions(lv_demo_truck_ui PUBLIC LV_USE_DEMO_ECG)
target_compile_features(lv_demo_truck_ui PUBLIC c_std_99)

target_include_directories(
  lv_demo_truck_ui
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(lv_demo_truck_ui PUBLIC lvgl)
