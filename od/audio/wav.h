#pragma once

#include <stdint.h>

#define WAV_FORMAT_PCM 1
#define WAV_FORMAT_FLOAT 3
#define WAV_FORMAT_EXTENSIBLE 0xFFFE /* SubFormat */

namespace od
{

	typedef struct _GUID
	{
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t Data4[8];
	} GUID;

	typedef struct
	{
		uint16_t wFormatTag;
		uint16_t nChannels;
		uint32_t nSamplesPerSec;
		uint32_t nAvgBytesPerSec;
		uint16_t nBlockAlign;
		uint16_t wBitsPerSample;
	} WavFormatData;

	typedef struct
	{
		uint16_t wFormatTag;
		uint16_t nChannels;
		uint32_t nSamplesPerSec;
		uint32_t nAvgBytesPerSec;
		uint16_t nBlockAlign;
		uint16_t wBitsPerSample;
		// extended portion
		uint16_t cbSize;
		uint16_t wValidBitsPerSample;
		uint32_t dwChannelMask;
		GUID SubFormat;
	} WavFormatExData;

	typedef struct
	{
		uint8_t ckID[4];
		uint32_t cksize;
	} RiffChunk;

	typedef struct
	{
		uint8_t ckID[4];
		uint32_t cksize;
		uint8_t WAVEID[4];
	} FileTypeChunk;

	typedef struct {
		uint32_t numCuePoints;
	} CueChunkData;

	typedef struct
	{
		uint8_t id[4];
		uint32_t position;
		uint8_t dataChunkId[4];
		uint32_t chunkStart;
		uint32_t blockStart;
		uint32_t sampleOffset;
	} CueFormatData;

} /* namespace od */
