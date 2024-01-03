/* Mixer.c
*  by Turi Scandurra
*  2024.01.03
*
*  The mixer logic is heavily based on the work of Ricardo Massaro: https://github.com/moefh/pico-audio-demo
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
  source->volume = 1024;
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
      if (! mixer_sources[i].active) continue;
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