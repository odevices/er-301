#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <od/graphics/fonts/font.h>

#define FONT_USE_WIDTH_TO_ADVANCE 0

#ifdef BUILDOPT_SBL

	const font_t *pixelsix48_lookup(int id);
	const font_family_t *pixelsix48_info(int id);

	static inline const font_t *font_lookup_default(int size, int id)
	{
		return pixelsix48_lookup(id);
	}

	static inline const font_t *font_lookup(int size, int id)
	{
		return pixelsix48_lookup(id);
	}

	static inline const font_family_t *font_info_default(int id)
	{
		return pixelsix48_info(id);
	}

	static inline const font_family_t *font_info(int size, int id)
	{
		return pixelsix48_info(id);
	}

#else

const font_t *consolas8_lookup(int id);
const font_family_t *consolas8_info(int id);

const font_t *consolas12b_lookup(int id);
const font_family_t *consolas12b_info(int id);

const font_t *pixelsix9_lookup(int id);
const font_family_t *pixelsix9_info(int id);

const font_t *courier_new16_lookup(int id);
const font_family_t *courier_new16_info(int id);

const font_t *pixelsix48_lookup(int id);
const font_family_t *pixelsix48_info(int id);

static inline const font_t *font_lookup_default(int size, int id)
{
	return consolas12b_lookup(id);
}

static inline const font_t *font_lookup(int size, int id)
{
	switch (size)
	{
	case 48:
		return pixelsix48_lookup(id);
	case 16:
		return courier_new16_lookup(id);
	case 12:
		return consolas12b_lookup(id);
	case 10:
		return pixelsix9_lookup(id);
	case 8:
		return consolas8_lookup(id);
	default:
		return pixelsix9_lookup(id);
	}
}

static inline const font_family_t *font_info_default(int id)
{
	return consolas12b_info(id);
}

static inline const font_family_t *font_info(int size, int id)
{
	switch (size)
	{
	case 48:
		return pixelsix48_info(id);
	case 16:
		return courier_new16_info(id);
	case 12:
		return consolas12b_info(id);
	case 10:
		return pixelsix9_info(id);
	case 8:
		return consolas8_info(id);
	default:
		return pixelsix9_info(id);
	}
}

#endif

#ifdef __cplusplus
}
#endif
