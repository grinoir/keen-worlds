/*
Render an IMF tune (OPL2 register, value, 16-bit delay in 1/560 s ticks) to a
WAV file with the engine's own sound chip emulator, to hear a tune without
starting the game.

  gcc -O2 -o /tmp/kw_imf2wav tools/kw_imf2wav.c omnispeak/src/opl/dbopl.c -Iomnispeak/src/opl -lm
  /tmp/kw_imf2wav tune.imf tune.wav [times through, default 1]

Prints how loud it gets (the peak, of 32767) and how much of it is silence.
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbopl.h"

#define RATE 44100

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "usage: kw_imf2wav tune.imf tune.wav [times]\n");
		return 1;
	}
	FILE *f = fopen(argv[1], "rb");
	if (!f)
		return perror(argv[1]), 1;
	static uint8_t imf[1 << 20];
	size_t size = fread(imf, 1, sizeof(imf), f);
	fclose(f);
	int times = argc > 3 ? atoi(argv[3]) : 1;

	static Chip chip;
	DBOPL_InitTables();
	Chip__Chip(&chip);
	Chip__Setup(&chip, RATE);

	size_t cap = 1 << 22, n = 0;
	int16_t *out = malloc(cap * sizeof(int16_t));
	double clock = 0; // samples owed
	int peak = 0;
	size_t quiet = 0;
	for (int pass = 0; pass < times; ++pass)
		for (size_t i = 0; i + 3 < size; i += 4)
		{
			Chip__WriteReg(&chip, imf[i], imf[i + 1]);
			clock += (imf[i + 2] | imf[i + 3] << 8) * (double)RATE / 560.0;
			while (clock >= 1)
			{
				Bit32s buf[512];
				int want = clock > 512 ? 512 : (int)clock;
				Chip__GenerateBlock2(&chip, want, buf);
				clock -= want;
				if (n + want > cap)
					out = realloc(out, (cap *= 2) * sizeof(int16_t));
				for (int k = 0; k < want; ++k)
				{
					int v = buf[k] * 2; // as the engine mixes it
					if (v > 32767) v = 32767;
					if (v < -32768) v = -32768;
					if (abs(v) > peak) peak = abs(v);
					if (abs(v) < 200) ++quiet;
					out[n++] = (int16_t)v;
				}
			}
		}

	f = fopen(argv[2], "wb");
	if (!f)
		return perror(argv[2]), 1;
	uint32_t bytes = (uint32_t)(n * 2), u;
	uint16_t s;
	fwrite("RIFF", 1, 4, f); u = 36 + bytes; fwrite(&u, 4, 1, f);
	fwrite("WAVEfmt ", 1, 8, f); u = 16; fwrite(&u, 4, 1, f);
	s = 1; fwrite(&s, 2, 1, f); s = 1; fwrite(&s, 2, 1, f);
	u = RATE; fwrite(&u, 4, 1, f); u = RATE * 2; fwrite(&u, 4, 1, f);
	s = 2; fwrite(&s, 2, 1, f); s = 16; fwrite(&s, 2, 1, f);
	fwrite("data", 1, 4, f); fwrite(&bytes, 4, 1, f);
	fwrite(out, 2, n, f);
	fclose(f);
	printf("%s: %.1f s, peak %d of 32767, %.0f%% near silence\n", argv[2], n / (double)RATE, peak, 100.0 * quiet / (n ? n : 1));
	return 0;
}
