cmake_minimum_required(VERSION 3.16)

idf_component_register(
        SRCS "main.c" "ssd1306.c" "Graphics.c"
        INCLUDE_DIRS "." "public" "private" "structure"
        REQUIRES driver esp_driver_i2c esp_adc esp_driver_gpio esp_timer esp_system log
        )
