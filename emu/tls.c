#include <emu/tls.h>
#include <SDL2/SDL.h>

static SDL_SpinLock tls_lock = 0;
static SDL_TLSID thread_local_storage = 0;
static SDL_TLSID thread_name = 0;

void TLS_set(void *value)
{
  if (!thread_local_storage)
  {
    SDL_AtomicLock(&tls_lock);
    if (!thread_local_storage)
    {
      thread_local_storage = SDL_TLSCreate();
    }
    SDL_AtomicUnlock(&tls_lock);
  }
  SDL_TLSSet(thread_local_storage, value, 0);
}

void *TLS_get(void)
{
  return SDL_TLSGet(thread_local_storage);
}

void TLS_setName(const char *value)
{
  if (!thread_name)
  {
    SDL_AtomicLock(&tls_lock);
    if (!thread_name)
    {
      thread_name = SDL_TLSCreate();
    }
    SDL_AtomicUnlock(&tls_lock);
  }
  SDL_TLSSet(thread_name, value, 0);
}

const char *TLS_getName()
{
  return SDL_TLSGet(thread_name);
}