/* RP2040 I2S Audio Mixer library example

Connections
PICO_AUDIO_I2S_DATA_PIN (28)          -> I2S DIN
PICO_AUDIO_I2S_CLOCK_PIN_BASE (26)    -> I2S BCK
PICO_AUDIO_I2S_CLOCK_PIN_BASE+1 (27)  -> I2S LRCK

*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "mixer.h"

#include "drums.h" // 22050Hz, 16bit headless wav

struct audio_buffer_pool *ap;

int main() {
    stdio_init_all();

    ap = init_audio();

    int sample_1 = audio_play_loop(drums, sizeof(drums), 0);

    // Other available functions are:
    // int sample_2 = audio_play_once(drums, sizeof(drums));
    // audio_source_stop(sample_2);
    // audio_source_set_volume(sample_2, 2048);

    while (true) {
      audio_i2s_step(ap);
    }

    return 0;
}



