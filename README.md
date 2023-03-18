# RP2040 I2S Audio Mixer library

This C library allows playback of audio samples on Raspberry Pi Pico through an I2S DAC.
It's set up to play 16bit mono samples at 22050Hz.
Multiple samples can play simultaneously, and their volume can be adjusted individually.

It requires [pico-extras](https://github.com/raspberrypi/pico-extras) as the I2S functionality is provided by the audio_i2s library by the Rasperry Pi Foundation.
The mixer logic is heavily based on the work of Ricardo Massaro for [pico-audio-demo](https://github.com/moefh/pico-audio-demo), release under the MIT License.
