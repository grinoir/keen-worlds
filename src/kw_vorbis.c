/*
Keen Worlds: Ogg Vorbis voice clips.

The voice clips (data/voice) are kept as Ogg Vorbis files, about a twelfth
of their size as plain samples. They are decoded when played, with
stb_vorbis (Sean Barrett's decoder, in the public domain; stb_vorbis.c
beside this file, unchanged).

Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdint.h>
#include <stdlib.h>

// Its own file is compiled here, quietly: it is not ours to tidy.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#include "stb_vorbis.c"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

// A whole Ogg Vorbis file in memory -> its samples (16-bit, one channel),
// to be freed by the caller; NULL if it can't be read or isn't mono.
int16_t *KW_DecodeVorbis(const uint8_t *data, int length, long *samples, int *rate)
{
	int channels = 0, sampleRate = 0;
	short *output = NULL;
	int count = stb_vorbis_decode_memory(data, length, &channels, &sampleRate, &output);
	if (count <= 0 || channels != 1)
	{
		free(output);
		return NULL;
	}
	*samples = count;
	*rate = sampleRate;
	return (int16_t *)output;
}
