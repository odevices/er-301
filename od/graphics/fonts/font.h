#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		int line_height;
		int base;
		const char *name;
	} font_family_t;

	typedef struct
	{
		int width;
		int height;
		int xadvance;
		int xoffset;
		int yoffset;
		const uint8_t *bitmap;
	} font_t;

#ifdef __cplusplus
}
#endif
