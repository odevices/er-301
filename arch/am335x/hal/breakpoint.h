#pragma once

#ifdef BUILDOPT_TESTING
#define Breakpoint() __asm__("BKPT")
#else
#define Breakpoint()
#endif
