#ifndef MIXER_H
#define MIXER_H

#define BUFFER_SIZE 256
#define AUDIO_MAX_SOURCES 4

struct MIXER_SOURCE {
  const uint16_t *samples;
  int len;//TODO different?
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
struct audio_buffer_pool *init_audio();
void audio_i2s_step(struct audio_buffer_pool *ap);

#endif /* MIXER_H */