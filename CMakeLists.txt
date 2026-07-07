cmake_minimum_required(VERSION 4.2)
project(ADC_Sensor_Processor C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(ADC_Sensor_Processor
        main.c
        adc.c
        io.c
        stats.c)

target_link_libraries(ADC_Sensor_Processor m)