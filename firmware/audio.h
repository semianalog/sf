#ifndef AUDIO_H
#define AUDIO_H

#include <inttypes.h>

/**
 * Initialize both timers for audio output (and incidentally the charge pump
 * timer, as it shares hardware), and begin emitting a tone.
 */
void init_audio(void);

/**
 * Set the frequency of audio output in Hz.
 */
void set_audio_freq(uint16_t freq);

/**
 * Set the volume of audio output, with 0 = 0% and 255 = 100%.
 */
void set_audio_volume(uint8_t vol);

#endif // AUDIO_H
