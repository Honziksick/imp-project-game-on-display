cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.c")

idf_component_register(
        SRCS ${SRC_FILES}
        INCLUDE_DIRS "." "enum" "public" "source" "structure"
        REQUIRES driver esp_driver_i2c esp_adc esp_driver_gpio esp_timer esp_system log
        )
