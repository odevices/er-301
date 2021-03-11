#pragma once

#ifdef BUILDOPT_TESTING
#include <signal.h>
#define Breakpoint() raise(SIGINT)
#else
#define Breakpoint()
#endif
