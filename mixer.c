/**
 * @file mixer.c
 * @brief Mixer logic implementation
 * @author Turi Scandurra
 * @date 2024.01.03
 * @note Heavily based on the work of Ricardo Massaro: https://github.com/moefh/pico-audio-demo
 */

#include "pico/stdlib.h"
#include "mixer.h"
#include "sound_i2s.h"
#include <string.h>
#include <stdlib.h>
#include "hardware/dma.h"
#if PICO_ON_DEVICE
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#endif

/**
 * @brief Claim an unused audio source
 * @return The index of the claimed source, or -1 if no source is available
 */
int audio_claim_unused_source(void)
{
    for (int i = 0; i < AUDIO_MAX_SOURCES; i++) {
        if (!mixer_sources[i].active) {
            mixer_sources[i].active = true;
            return i;
        }
    }
    return -1;
}

/**
 * @brief Play a sound once
 * @param samples The sound samples to play
 * @param len The length of the sound samples
 * @return The index of the playing source, or -1 if no source is available
 */
int audio_play_once(const uint16_t *samples, int len)
{
    int source_id = audio_claim_unused_source();
    if (source_id < 0) {
        return -1;
    }
    struct MIXER_SOURCE *source = &mixer_sources[source_id];
    source->samples = samples;
    source->len = len >> 1; // Divide by 2
    source->pos = 0;
    source->loop = false;
    source->volume = 1024;
    return source_id;
}

/**
 * @brief Play a sound in a loop
 * @param samples The sound samples to play
 * @param len The length of the sound samples
 * @param loop_start The starting position of the loop
 * @return The index of the playing source, or -1 if no source is available
 */
int audio_play_loop(const uint16_t *samples, int len, int loop_start)
{
    int source_id = audio_play_once(samples, len);
    if (source_id < 0) {
        return -1;
    }
    struct MIXER_SOURCE *source = &mixer_sources[source_id];
    source->loop = true;
    source->loop_start = loop_start;
    return source_id;
}

/**
 * @brief Stop a playing sound source
 * @param source_id The index of the sound source to stop
 */
void audio_source_stop(int source_id)
{
    mixer_sources[source_id].active = false;
}

/**
 * @brief Set the volume of a playing sound source
 * @param source_id The index of the sound source to set the volume for
 * @param volume The new volume value (8.8 fixed point)
 */
void audio_source_set_volume(int source_id, uint16_t volume)
{
    mixer_sources[source_id].volume = volume;
}

/**
 * @brief Perform a single step of the audio mixer
 */
void audio_i2s_step()
{
    static int16_t *last_buffer;
    int16_t *buffer = sound_i2s_get_next_buffer();
    if (buffer == NULL) return;
    if (buffer != last_buffer) {
        last_buffer = buffer;

        // Mixer step
        memset(mixer_buffer, 0, sizeof(mixer_buffer));
        for (uint i = 0; i < AUDIO_MAX_SOURCES; i++) {
            if (!mixer_sources[i].active) continue;
            struct MIXER_SOURCE *source = &mixer_sources[i];

            int remain_pos = source->len - source->pos;
            if (remain_pos > BUFFER_SIZE) {
                remain_pos = BUFFER_SIZE;
            }
            for (uint i = 0; i < remain_pos; i++) {
                mixer_buffer[i] += ((source->samples[i+source->pos]-1024) * source->volume) >> 8;
            }
            source->pos += remain_pos;

            // Handle source termination
            if (source->pos == source->len) {
                if (source->loop) {
                    source->pos = source->loop_start;
                } else {
                    source->active = false;
                }
            }
        }
        
        // Copy to I2S buffer
        for (int i = 0; i < SOUND_I2S_BUFFER_NUM_SAMPLES; i++) {
            *buffer++ = mixer_buffer[i];
            *buffer++ = mixer_buffer[i];
        }
    }
}

