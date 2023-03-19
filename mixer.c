/* Mixer.c
*  by Turi Scandurra 2023.03.18
*
*  The mixer logic is heavily based on the work of Ricardo Massaro: https://github.com/moefh/pico-audio-demo
*/

#include "pico/stdlib.h"
#include "mixer.h"
#include "pico/audio_i2s.h"
#include <string.h>
#include "hardware/dma.h"
#include <math.h>
#if PICO_ON_DEVICE
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#endif

struct audio_buffer *buffer;
int16_t *samples;

int audio_claim_unused_source(void)
{
  for (int i = 0; i < AUDIO_MAX_SOURCES; i++) {
    if (! mixer_sources[i].active) {
      mixer_sources[i].active = true;
      return i;
    }
  }
  return -1;
}

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
  source->volume = 128; // TODO 8>16 2048
  return source_id;
}

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

void audio_source_stop(int source_id)
{
  mixer_sources[source_id].active = false;
}

void audio_source_set_volume(int source_id, uint16_t volume)
{
  mixer_sources[source_id].volume = volume;
}

void audio_i2s_step(struct audio_buffer_pool *ap){
    buffer = take_audio_buffer(ap, true);
    if (buffer == NULL) return;
    samples = (uint16_t *) buffer->buffer->bytes;

    // Mixer step
    memset(mixer_buffer, 0, sizeof(mixer_buffer));

    for (uint i = 0; i < AUDIO_MAX_SOURCES; i++) {
      if (! mixer_sources[i].active) continue;
      struct MIXER_SOURCE *source = &mixer_sources[i];

      int remain_pos = source->len - source->pos;
      if (remain_pos > BUFFER_SIZE) {
        remain_pos = BUFFER_SIZE;
      }
      for (uint i = 0; i < remain_pos; i++) {
        mixer_buffer[i] += ((source->samples[i+source->pos]-128) * source->volume) >> 8; // TODO bitwise // TODO 8>16
      }
      source->pos += remain_pos;

      // handle source termination
      if (source->pos == source->len) {
        if (source->loop) {
          source->pos = source->loop_start;
        } else {
          source->active = false;
        }
      }
    }

    for (uint i = 0; i < BUFFER_SIZE; i++) {
        // TODO 8>16
        // convert 32-bit mixer buffer to 16-bit output buffer
        //uint16_t sample = mixer_buffer[i];
        samples[i] = mixer_buffer[i];
    }
    buffer->sample_count = buffer->max_sample_count;

    give_audio_buffer(ap, buffer);
}

struct audio_buffer_pool *init_audio() {

    static audio_format_t audio_format = {
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .sample_freq = 22050,
            .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 1,
                                                                      BUFFER_SIZE); // todo correct size
    bool __unused ok;
    const struct audio_format *output_format;
    struct audio_i2s_config config = {
            .data_pin = PICO_AUDIO_I2S_DATA_PIN,
            .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
            .dma_channel = 0,
            .pio_sm = 0,
    };

    output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }

    ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);

    return producer_pool;
}