cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

project(i2s_mixer C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
        example.c
        )

target_sources(${PROJECT_NAME} PRIVATE
        mixer.c
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        lib/RP2040-I2S-Mixer
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
        pico_stdlib
        hardware_dma
        pico_audio_i2s
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE
        # compile time configuration of I2S
        PICO_AUDIO_I2S_MONO_INPUT=1
        # PICO_AUDIO_I2S_DATA_PIN=28
        # PICO_AUDIO_I2S_CLOCK_PIN_BASE=26
        )

pico_add_extra_outputs(${PROJECT_NAME})