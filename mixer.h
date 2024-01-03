#ifndef MIXER_H
#define MIXER_H

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 1024
#define AUDIO_MAX_SOURCES 6

struct MIXER_SOURCE {
  const uint16_t *samples;
  int len;
  int loop_start;
  int pos;
  bool active;
  bool loop;
  uint16_t volume; // 8.8 fixed point
};

static struct MIXER_SOURCE mixer_sources[AUDIO_MAX_SOURCES];
static int32_t mixer_buffer[BUFFER_SIZE];

int audio_claim_unused_source(void);
int audio_play_once(const uint16_t *samples, int len);
int audio_play_loop(const uint16_t *samples, int len, int loop_start);
void audio_source_stop(int source_id);
void audio_source_set_volume(int source_id, uint16_t volume);
void mixer_init(uint8_t max_channels);
void audio_i2s_step();

#ifdef __cplusplus
}
#endif

#endif /* MIXER_H */