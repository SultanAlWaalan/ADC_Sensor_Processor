cmake_minimum_required(VERSION 4.2)
project(ADC_Sensor_Processor C)

set(CMAKE_C_STANDARD 99)

add_executable(ADC_Sensor_Processor main.c
        io.c
        io.h
        adc.c
        adc.h
        stats.c
        stats.h)
