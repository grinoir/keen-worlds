/*
Keen Worlds: making the player's Keen Dreams readable by the engine.

Keen Dreams (shareware version 1.13) keeps the directories and dictionaries
of its graphics, sounds and levels inside its program, and packs its levels
with Huffman and RLEW where Keen 4-6 use Carmack and RLEW. The engine reads
Keen 4-6's kind of files, so the player's copy is written once more in that
form, into the dream's own folder, whenever any of those files is missing:

	KDREAMS.EXE  ->  EGAHEAD, EGADICT, MAPHEAD, AUDIOHHD, AUDIODCT (.KDR)
	KDREAMS.MAP  ->  GAMEMAPS.KDR
	KDREAMS.EGA  ->  EGAGRAPH.KDR (a copy)
	KDREAMS.AUD  ->  AUDIO.KDR (a copy)

Nothing of the game is changed. This is kwtools/kw_dreams.py in C, so that
nobody needs Python to play; the two write the same files byte for byte.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "id_fs.h"
#include "ck_cross.h"

// Version 1.13 (shareware): where the built-in files are in the unpacked
// program (the places ReflectionHLE gives for this version).
#define KD_EXE_SIZE 81619
#define KD_EGAHEAD_AT 0x1cb20
#define KD_EGAHEAD_SIZE 12068
#define KD_EGADICT_AT 0x29846
#define KD_MAPHEAD_AT 0x1fa50
#define KD_MAPHEAD_SIZE 11824
#define KD_MAPDICT_AT 0x29c46
#define KD_AUDIOHHD_AT 0x22880
#define KD_AUDIOHHD_SIZE 340
#define KD_AUDIODCT_AT 0x2a042
#define KD_NUM_CHUNKS 3016
#define KD_RLEW_TAG 0xABCD
#define KD_IMAGE_MAX (1024 * 1024)

typedef struct KD_Bytes
{
	uint8_t *data;
	size_t size, room;
} KD_Bytes;

static void KD_Put(KD_Bytes *b, const void *src, size_t count)
{
	if (b->size + count > b->room)
	{
		b->room = (b->size + count) * 2 + 4096;
		b->data = (uint8_t *)realloc(b->data, b->room);
	}
	memcpy(b->data + b->size, src, count);
	b->size += count;
}

static void KD_Put16(KD_Bytes *b, unsigned value)
{
	uint8_t le[2] = {(uint8_t)value, (uint8_t)(value >> 8)};
	KD_Put(b, le, 2);
}

static void KD_Put32(KD_Bytes *b, uint32_t value)
{
	uint8_t le[4] = {(uint8_t)value, (uint8_t)(value >> 8), (uint8_t)(value >> 16), (uint8_t)(value >> 24)};
	KD_Put(b, le, 4);
}

static unsigned KD_Get16(const uint8_t *p)
{
	return p[0] | p[1] << 8;
}

static int32_t KD_Get32(const uint8_t *p)
{
	return (int32_t)(p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24);
}

static uint8_t *KD_ReadWhole(const char *dir, const char *name, size_t *size)
{
	FS_File f = FSL_OpenFileInDirCaseInsensitive(dir, name, false);
	if (!FS_IsFileValid(f))
		return NULL;
	*size = FS_GetFileSize(f);
	uint8_t *data = (uint8_t *)malloc(*size + 16);
	if (FS_Read(data, 1, *size, f) != *size)
	{
		free(data);
		data = NULL;
	}
	FS_CloseFile(f);
	return data;
}

// The size of KDREAMS.EXE in dir, or 0 if it isn't there.
static size_t KD_ExeSize(const char *dir)
{
	FS_File f = FSL_OpenFileInDirCaseInsensitive(dir, "KDREAMS.EXE", false);
	if (!FS_IsFileValid(f))
		return 0;
	size_t size = FS_GetFileSize(f);
	FS_CloseFile(f);
	return size;
}

// The folder holding the game: dir, or a folder up to two below it (a
// download often unpacks into a folder or two of its own). The four files
// are all read from that one folder, so that two copies side by side are
// never mixed. Otherwise the first KDREAMS.EXE of another version, so the
// player can be told what it is; false if there is none at all.
static bool KD_FindGame(const char *dir, int depth, char *found, size_t foundSize, size_t *otherSize)
{
	size_t size = KD_ExeSize(dir);
	if (size == KD_EXE_SIZE)
	{
		snprintf(found, foundSize, "%s", dir);
		return true;
	}
	if (size && !*otherSize)
		*otherSize = size;
	if (depth == 2)
		return false;
	DIR *d = opendir(dir);
	if (!d)
		return false;
	bool ok = false;
	for (struct dirent *entry = readdir(d); entry && !ok; entry = readdir(d))
	{
		if (entry->d_name[0] == '.')
			continue;
		char inner[512];
		snprintf(inner, sizeof(inner), "%s/%s", dir, entry->d_name);
		ok = KD_FindGame(inner, depth + 1, found, foundSize, otherSize);
	}
	closedir(d);
	return ok;
}

static bool KD_WriteWhole(const char *dir, const char *name, const void *data, size_t size)
{
	// Over a file of the name in any case, so there is only ever one.
	FILE *f = FSL_OpenFileInDirCaseInsensitive(dir, name, true);
	if (!f)
	{
		char path[512];
		snprintf(path, sizeof(path), "%s/%s", dir, name);
		f = fopen(path, "wb");
	}
	if (!f)
		return false;
	bool ok = fwrite(data, 1, size, f) == size;
	return fclose(f) == 0 && ok;
}

// Unpacks an LZEXE 0.91 program into its load image (code and data, no
// header). Returns its size, or 0 if the file isn't one.
static size_t KD_Unlzexe(const uint8_t *d, size_t dlen, uint8_t *out)
{
	if (dlen < 32)
		return 0;
	size_t hdr = KD_Get16(d + 8) * 16, cs = KD_Get16(d + 22);
	size_t infoAt = hdr + cs * 16;
	if (infoAt + 16 > dlen || KD_Get16(d + infoAt + 8) > cs)
		return 0;
	size_t p = hdr + (cs - KD_Get16(d + infoAt + 8)) * 16, n = 0;
	if (p + 2 > dlen)
		return 0;
	unsigned buf = KD_Get16(d + p);
	int cnt = 16;
	p += 2;
#define KD_NEED(bytes) \
	if (p + (bytes) > dlen) \
		return 0
#define KD_BIT(into) \
	{ \
		into = buf & 1; \
		if (--cnt == 0) \
		{ \
			KD_NEED(2); \
			buf = KD_Get16(d + p); \
			p += 2; \
			cnt = 16; \
		} \
		else \
			buf >>= 1; \
	}
	for (;;)
	{
		int b, span, len;
		KD_BIT(b);
		if (b)
		{
			KD_NEED(1);
			if (n >= KD_IMAGE_MAX)
				return 0;
			out[n++] = d[p++];
			continue;
		}
		KD_BIT(b);
		if (!b)
		{
			int hi, lo;
			KD_BIT(hi);
			KD_BIT(lo);
			len = (hi << 1 | lo) + 2;
			KD_NEED(1);
			span = d[p++] - 256;
		}
		else
		{
			KD_NEED(2);
			int lo = d[p++], hi = d[p++];
			span = (lo | ((hi & ~7) << 5) | 0xE000) - 0x10000;
			len = (hi & 7) + 2;
			if (len == 2)
			{
				KD_NEED(1);
				len = d[p++];
				if (len == 0)
					break;
				if (len == 1)
					continue;
				len++;
			}
		}
		if ((long)n + span < 0 || n + len > KD_IMAGE_MAX)
			return 0;
		for (int i = 0; i < len; i++, n++)
			out[n] = out[(long)n + span];
	}
#undef KD_BIT
#undef KD_NEED
	return n;
}

// Huffman as Keen Dreams packs its levels: 255 nodes of two words each,
// starting from the last. False if the data ends first.
static bool KD_HuffExpand(const uint8_t *src, size_t srcLen, const uint8_t *nodes, uint8_t *out, size_t length)
{
	size_t p = 0, n = 0;
	int node = 254, bit = 0;
	while (n < length)
	{
		if (p >= srcLen)
			return false;
		unsigned v = KD_Get16(nodes + (node * 2 + ((src[p] >> bit) & 1)) * 2);
		if (++bit == 8)
		{
			bit = 0;
			p++;
		}
		if (v < 256)
		{
			out[n++] = (uint8_t)v;
			node = 254;
		}
		else
		{
			node = v - 256;
			if (node > 254)
				return false;
		}
	}
	return true;
}

// One plane of a level in Keen 4-6's form: its size, then RLEW data in
// Carmack's format without Carmack's compression (its two marker bytes
// escaped).
static void KD_PutPlane(KD_Bytes *out, const uint16_t *words, size_t count)
{
	KD_Bytes body = {NULL, 0, 0}, packed = {NULL, 0, 0};
	KD_Put16(&body, (unsigned)(count * 2));
	for (size_t i = 0; i < count;)
	{
		size_t j = i;
		while (j < count && words[j] == words[i] && j - i < 0xFFFF)
			j++;
		if (j - i > 3 || words[i] == KD_RLEW_TAG)
		{
			KD_Put16(&body, KD_RLEW_TAG);
			KD_Put16(&body, (unsigned)(j - i));
			KD_Put16(&body, words[i]);
		}
		else
		{
			for (size_t k = i; k < j; k++)
				KD_Put16(&body, words[k]);
		}
		i = j;
	}
	for (size_t i = 0; i < body.size; i += 2)
	{
		uint8_t lo = body.data[i], hi = body.data[i + 1];
		if (hi == 0xA7 || hi == 0xA8)
		{
			uint8_t escaped[3] = {0, hi, lo};
			KD_Put(&packed, escaped, 3);
		}
		else
			KD_Put(&packed, body.data + i, 2);
	}
	KD_Put16(out, (unsigned)body.size);
	KD_Put(out, packed.data, packed.size);
	free(body.data);
	free(packed.data);
}

// The levels, unpacked and written again. False if KDREAMS.MAP doesn't fit
// the program's directory.
static bool KD_ConvertLevels(const uint8_t *image, const uint8_t *maps, size_t mapsLen, KD_Bytes *gamemaps, KD_Bytes *maphead)
{
	const uint8_t *head = image + KD_MAPHEAD_AT, *nodes = image + KD_MAPDICT_AT;
	unsigned tag = KD_Get16(head);
	int32_t offsets[100] = {0};
	KD_Put(gamemaps, "TED5v1.0", 8);
	for (int n = 0; n < 100; n++)
	{
		int32_t at = KD_Get32(head + 2 + n * 4);
		if (at <= 0)
			continue;
		uint8_t hdr[38];
		size_t hdrLen = head[402 + n];
		if ((size_t)at + hdrLen > mapsLen || !KD_HuffExpand(maps + at, hdrLen, nodes, hdr, sizeof(hdr)))
			return false;
		size_t width = KD_Get16(hdr + 18), height = KD_Get16(hdr + 20);
		uint32_t starts[3];
		unsigned lengths[3];
		for (int plane = 0; plane < 3; plane++)
		{
			int32_t start = KD_Get32(hdr + plane * 4);
			size_t length = KD_Get16(hdr + 12 + plane * 2);
			if (start < 0 || length < 2 || (size_t)start + length > mapsLen)
				return false;
			// Huffman holds RLEW data; its first word is the size unpacked.
			size_t expanded = KD_Get16(maps + start);
			uint8_t *rlew = (uint8_t *)malloc(expanded + 2);
			uint16_t *words = (uint16_t *)calloc(width * height + 1, sizeof(uint16_t));
			bool ok = KD_HuffExpand(maps + start + 2, length - 2, nodes, rlew, expanded);
			size_t have = 0, numWords = expanded / 2;
			for (size_t i = 1; ok && have < width * height; )
			{
				if (i >= numWords)
				{
					ok = false;
					break;
				}
				unsigned w = KD_Get16(rlew + i * 2);
				i++;
				if (w == tag)
				{
					if (i + 1 >= numWords)
					{
						ok = false;
						break;
					}
					unsigned run = KD_Get16(rlew + i * 2), value = KD_Get16(rlew + i * 2 + 2);
					i += 2;
					while (run-- && have < width * height)
						words[have++] = (uint16_t)value;
				}
				else
					words[have++] = (uint16_t)w;
			}
			if (ok)
			{
				starts[plane] = (uint32_t)gamemaps->size;
				KD_PutPlane(gamemaps, words, width * height);
				lengths[plane] = (unsigned)(gamemaps->size - starts[plane]);
			}
			free(rlew);
			free(words);
			if (!ok)
				return false;
		}
		offsets[n] = (int32_t)gamemaps->size;
		for (int plane = 0; plane < 3; plane++)
			KD_Put32(gamemaps, starts[plane]);
		for (int plane = 0; plane < 3; plane++)
			KD_Put16(gamemaps, lengths[plane]);
		KD_Put16(gamemaps, (unsigned)width);
		KD_Put16(gamemaps, (unsigned)height);
		char name[16] = {0};
		for (int i = 0; i < 15 && hdr[22 + i]; i++)
			name[i] = (char)hdr[22 + i];
		KD_Put(gamemaps, name, 16);
		KD_Put(gamemaps, "!ID!", 4);
	}
	// The tile properties follow the map directory in the program; the
	// engine takes them after the map header.
	KD_Put16(maphead, KD_RLEW_TAG);
	for (int n = 0; n < 100; n++)
		KD_Put32(maphead, (uint32_t)offsets[n]);
	KD_Put(maphead, head + 502, KD_MAPHEAD_SIZE - 502);
	return true;
}

// What the engine reads in place of the game, all written into the dream's
// folder. The dream is there when every one of them is.
static const char *const kd_madeFiles[] = {
	"EGAGRAPH.KDR", "EGAHEAD.KDR", "EGADICT.KDR", "AUDIO.KDR",
	"AUDIOHHD.KDR", "AUDIODCT.KDR", "MAPHEAD.KDR", "GAMEMAPS.KDR"};

bool KD_IsConverted(const char *dir)
{
	for (size_t i = 0; i < sizeof(kd_madeFiles) / sizeof(*kd_madeFiles); i++)
	{
		FS_File f = FSL_OpenFileInDirCaseInsensitive(dir, kd_madeFiles[i], false);
		if (!FS_IsFileValid(f))
			return false;
		FS_CloseFile(f);
	}
	return true;
}

// Writes the engine's form of the Keen Dreams found in (or up to two folders
// below) dir into dir, over whatever is there. Returns NULL when done, or
// what is wrong, in lines short enough for a card in the game.
const char *KD_ConvertGame(const char *dir)
{
	static char problemText[160];
	char src[512];
	size_t otherSize = 0;
	if (!KD_FindGame(dir, 0, src, sizeof(src), &otherSize))
	{
		if (!otherSize)
			return "KDREAMS.EXE is not in\nmods/dreams.";
		snprintf(problemText, sizeof(problemText),
			"KDREAMS.EXE is %u bytes:\nanother version. This needs\nshareware 1.13 (81619).",
			(unsigned)otherSize);
		return problemText;
	}

	size_t exeLen = 0, mapsLen = 0, egaLen = 0, audLen = 0;
	uint8_t *exe = KD_ReadWhole(src, "KDREAMS.EXE", &exeLen);
	uint8_t *maps = KD_ReadWhole(src, "KDREAMS.MAP", &mapsLen);
	uint8_t *ega = KD_ReadWhole(src, "KDREAMS.EGA", &egaLen);
	uint8_t *aud = KD_ReadWhole(src, "KDREAMS.AUD", &audLen);
	uint8_t *image = (uint8_t *)calloc(KD_IMAGE_MAX, 1);
	KD_Bytes egahead = {NULL, 0, 0}, gamemaps = {NULL, 0, 0}, maphead = {NULL, 0, 0}, audiohhd = {NULL, 0, 0};
	const char *problem = NULL;

	if (!exe || !maps || !ega || !aud)
		problem = "KDREAMS.MAP, .EGA or .AUD\nis missing beside\nKDREAMS.EXE.";
	else if (KD_Unlzexe(exe, exeLen, image) < KD_AUDIODCT_AT + 1024)
		problem = "KDREAMS.EXE could not\nbe unpacked.";

	if (!problem)
	{
		// The graphics directory's entries are four bytes each there, three here.
		for (int i = 0; i <= KD_NUM_CHUNKS; i++)
		{
			int32_t at = KD_Get32(image + KD_EGAHEAD_AT + i * 4);
			uint32_t value = at < 0 ? 0xFFFFFF : (uint32_t)at;
			uint8_t three[3] = {(uint8_t)value, (uint8_t)(value >> 8), (uint8_t)(value >> 16)};
			KD_Put(&egahead, three, 3);
		}
		if ((size_t)KD_Get32(image + KD_EGAHEAD_AT + KD_NUM_CHUNKS * 4) != egaLen)
			problem = "KDREAMS.EGA does not\nfit this KDREAMS.EXE.";
	}
	if (!problem && !KD_ConvertLevels(image, maps, mapsLen, &gamemaps, &maphead))
		problem = "KDREAMS.MAP does not\nfit this KDREAMS.EXE.";
	if (!problem)
	{
		// The directory's entries for the digitized sounds the game doesn't
		// have point past the end of the file: an empty chunk starts where
		// it ends.
		for (int i = 0; i < KD_AUDIOHHD_SIZE / 4; i++)
		{
			uint32_t at = (uint32_t)KD_Get32(image + KD_AUDIOHHD_AT + i * 4);
			KD_Put32(&audiohhd, at > audLen ? (uint32_t)audLen : at);
		}
		// The levels last: they are what says the dream is installed.
		if (!KD_WriteWhole(dir, "EGAGRAPH.KDR", ega, egaLen)
			|| !KD_WriteWhole(dir, "EGAHEAD.KDR", egahead.data, egahead.size)
			|| !KD_WriteWhole(dir, "EGADICT.KDR", image + KD_EGADICT_AT, 1024)
			|| !KD_WriteWhole(dir, "AUDIO.KDR", aud, audLen)
			|| !KD_WriteWhole(dir, "AUDIOHHD.KDR", audiohhd.data, audiohhd.size)
			|| !KD_WriteWhole(dir, "AUDIODCT.KDR", image + KD_AUDIODCT_AT, 1024)
			|| !KD_WriteWhole(dir, "MAPHEAD.KDR", maphead.data, maphead.size)
			|| !KD_WriteWhole(dir, "GAMEMAPS.KDR", gamemaps.data, gamemaps.size))
			problem = "mods/dreams can't be\nwritten to.";
	}

	free(exe);
	free(maps);
	free(ega);
	free(aud);
	free(image);
	free(egahead.data);
	free(gamemaps.data);
	free(maphead.data);
	free(audiohhd.data);
	return problem;
}
