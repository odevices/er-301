#ifndef __USB_DEBUG_H__
#define __USB_DEBUG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define BUILDOPT_VERBOSE
#define BUILDOPT_DEBUG_LEVEL 0
#define ASSERT(expr) logAssert(expr)
#define debug_printf logRaw
#include <hal/log.h>

#ifdef __cplusplus
}
#endif

#endif // __USB_DEBUG_H__