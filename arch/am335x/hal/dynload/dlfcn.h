#ifndef __DLFCN_H__
#define __DLFCN_H__

/* Dynamic linking */

/* Relocations are performed at an implementation-defined time. */
#define RTLD_LAZY 0x1
/* Relocations are performed when the object is loaded. */
#define RTLD_NOW 0x2
/* All symbols are not made available for relocation processing by other modules. */
#define RTLD_LOCAL 0x4
/* All symbols are available for relocation processing of other modules. */
#define RTLD_GLOBAL 0x8

#ifdef __cplusplus
extern "C"
{
#endif

    int dlclose(void *__handle);
    const char *dlerror(void);
    void *dlopen(const char *__path, int __mode);
    void *dlsym(void *__handle, const char *__symbol);

#ifdef __cplusplus
}
#endif

#endif // __DLFCN_H__