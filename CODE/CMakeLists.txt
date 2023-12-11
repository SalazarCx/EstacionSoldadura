cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(P_FINAL_JS_AD)

pico_sdk_init()

add_executable(P_FINAL_JS_AD ElecSolder.c)

pico_add_extra_outputs(P_FINAL_JS_AD)

target_include_directories(P_FINAL_JS_AD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(P_FINAL_JS_AD pico_stdlib hardware_gpio hardware_pwm hardware_irq pico_time hardware_adc)

pico_enable_stdio_usb(P_FINAL_JS_AD 1)
pico_enable_stdio_uart(P_FINAL_JS_AD 0)

pico_add_extra_outputs(P_FINAL_JS_AD)