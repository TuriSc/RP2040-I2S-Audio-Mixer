/**
 * @file example.c
 * @brief RP2040 I2S Audio Mixer library example
 * @author Turi Scandurra
 * @date 2024.01.03
 */

/*

Connections
I2S_DATA_PIN (28)          -> I2S DIN
I2S_CLOCK_PIN_BASE (26)    -> I2S BCK
I2S_CLOCK_PIN_BASE+1 (27)  -> I2S LRCK (BCK+1)

*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "mixer.h"

#include <string.h>
#include "hardware/gpio.h"
#include "sound_i2s.h"

#include "drums.h" // 22050Hz, 16bit headless wav

/**
 * @def I2S_DATA_PIN
 * @brief The pin used for I2S data
 */
#define I2S_DATA_PIN              28 // DIN

/**
 * @def I2S_CLOCK_PIN_BASE
 * @brief The base pin used for I2S clock
 */
#define I2S_CLOCK_PIN_BASE        26 // BCK

/**
 * @def SOUND_OUTPUT_FREQUENCY
 * @brief The frequency of the sound output
 */
#define SOUND_OUTPUT_FREQUENCY    22050

/**
 * @struct sound_i2s_config
 * @brief Configuration for the sound I2S interface
 */
static const struct sound_i2s_config sound_config = {
    /**
     * @brief The pin used for I2S data
     */
    .pin_sda         = I2S_DATA_PIN,
    /**
     * @brief The pin used for I2S clock
     */
    .pin_scl         = I2S_CLOCK_PIN_BASE,
    /**
     * @brief The pin used for I2S word select
     */
    .pin_ws          = I2S_CLOCK_PIN_BASE + 1,
    /**
     * @brief The frequency of the sound output
     */
    .sample_rate     = SOUND_OUTPUT_FREQUENCY,
    /**
     * @brief The number of bits per sample
     */
    .bits_per_sample = 16,
    /**
     * @brief The PIO number to use
     */
    .pio_num         = 0, // 0 for pio0, 1 for pio1
};

int main() {
    stdio_init_all();

    sound_i2s_init(&sound_config);
    sound_i2s_playback_start();

    int sample_1 = audio_play_loop(drums, sizeof(drums), 0);

    // Other available functions are:
    // int sample_2 = audio_play_once(drums, sizeof(drums));
    // audio_source_stop(sample_2);
    // audio_source_set_volume(sample_2, 2048);

    while (true) {
        audio_i2s_step();
        sleep_ms(5);
    }

    return 0;
}

