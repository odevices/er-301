#pragma once

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define CLAMP(x0, x1, x) MAX(x0, MIN(x1, x))
#define CLAMP_UNORDERED(x0, x1, x) MAX(MIN(x0, x1), MIN(MAX(x0, x1), x))
