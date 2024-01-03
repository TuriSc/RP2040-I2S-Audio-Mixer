# RP2040 I2S Audio Mixer library

This C library allows playback of audio samples on Raspberry Pi Pico through an I2S DAC.
Multiple samples can play simultaneously, and their volume can be adjusted individually.

An example program is included, configured to play 16bit mono samples at 22050Hz. The DAC used for this example is MAX98357A.

This code is heavily based on the work of Ricardo Massaro for [pico-audio-demo](https://github.com/moefh/pico-audio-demo) and [pico-mod-player](https://github.com/moefh/pico-mod-player). See [sound_i2s/LICENSE](/sound_i2s/LICENSE) for details.

### Version history
- 2024.01.03 - Removed 'Pico Extras' dependency. Breaking changes.
- 2023.12.16 - Fixed low volume issue
- 2023.03.18 - Initial release
