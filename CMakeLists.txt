if (NOT TARGET i2s_audio_mixer)
    add_library(i2s_audio_mixer INTERFACE)

    target_sources(i2s_audio_mixer INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/mixer.c
    )

    target_include_directories(i2s_audio_mixer INTERFACE ${CMAKE_CURRENT_LIST_DIR})

    target_link_libraries(i2s_audio_mixer INTERFACE
        pico_stdlib
        hardware_dma
        pico_audio_i2s
    )
endif()

### library file, tbf