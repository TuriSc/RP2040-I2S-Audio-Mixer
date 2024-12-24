/**
 * @file mixer.h
 * @brief Mixer logic interface
 * @author Turi Scandurra
 * @date 2024.01.03
 */

#ifndef MIXER_H
#define MIXER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def BUFFER_SIZE
 * @brief The size of the mixer buffer
 */
#define BUFFER_SIZE 1024

/**
 * @def AUDIO_MAX_SOURCES
 * @brief The maximum number of audio sources
 */
#define AUDIO_MAX_SOURCES 6

/**
 * @struct MIXER_SOURCE
 * @brief Represents an audio source
 */
struct MIXER_SOURCE {
    /**
     * @brief The sound samples for this source
     */
    const uint16_t *samples;
    /**
     * @brief The length of the sound samples
     */
    int len;
    /**
     * @brief The starting position of the loop
     */
    int loop_start;
    /**
     * @brief The current position in the sound samples
     */
    int pos;
    /**
     * @brief Whether this source is active
     */
    bool active;
    /**
     * @brief Whether this source should loop
     */
    bool loop;
    /**
     * @brief The volume of this source (8.8 fixed point)
     */
    uint16_t volume;
};

/**
 * @var mixer_sources
 * @brief An array of audio sources
 */
static struct MIXER_SOURCE mixer_sources[AUDIO_MAX_SOURCES];

/**
 * @var mixer_buffer
 * @brief The mixer buffer
 */
static int32_t mixer_buffer[BUFFER_SIZE];

/**
 * @brief Claim an unused audio source
 * @return The index of the claimed source, or -1 if no source is available
 */
int audio_claim_unused_source(void);

/**
 * @brief Play a sound once
 * @param samples The sound samples to play
 * @param len The length of the sound samples
 * @return The index of the playing source, or -1 if no source is available
 */
int audio_play_once(const uint16_t *samples, int len);

/**
 * @brief Play a sound in a loop
 * @param samples The sound samples to play
 * @param len The length of the sound samples
 * @param loop_start The starting position of the loop
 * @return The index of the playing source, or -1 if no source is available
 */
int audio_play_loop(const uint16_t *samples, int len, int loop_start);

/**
 * @brief Stop a playing sound source
 * @param source_id The index of the sound source to stop
 */
void audio_source_stop(int source_id);

/**
 * @brief Set the volume of a playing sound source
 * @param source_id The index of the sound source to set the volume for
 * @param volume The new volume value (8.8 fixed point)
 */
void audio_source_set_volume(int source_id, uint16_t volume);

/**
 * @brief Perform a single step of the audio mixer
 */
void audio_i2s_step();

/**
 * @brief Initialize the mixer
 * @param max_channels The maximum number of channels
 */
void mixer_init(uint8_t max_channels);

#ifdef __cplusplus
}
#endif

#endif /* MIXER_H */

