set(TARGET_NAME "i2s_audio_mixer")

if (NOT TARGET ${TARGET_NAME})
    add_library(${TARGET_NAME} INTERFACE)

    target_sources(${TARGET_NAME} INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/mixer.c
            ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s.c
    )

    pico_generate_pio_header(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_16bits.pio)
    pico_generate_pio_header(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_8bits.pio)

    target_include_directories(${TARGET_NAME} INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/sound_i2s
    )

    target_link_libraries(${TARGET_NAME} INTERFACE
        pico_stdlib
        hardware_dma
        hardware_pio
        hardware_irq
    )
endif()