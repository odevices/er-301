#include <hal/concurrency/Mutex.h>
#include <SDL2/SDL_mutex.h>

namespace od
{

  Mutex::Mutex()
  {
    mHandle = (void *)SDL_CreateMutex();
  }

  Mutex::~Mutex()
  {
    SDL_mutex *h = (SDL_mutex *)mHandle;
    SDL_DestroyMutex(h);
  }

  void Mutex::enter()
  {
    SDL_mutex *h = (SDL_mutex *)mHandle;
    SDL_LockMutex(h);
  }

  void Mutex::leave()
  {
    SDL_mutex *h = (SDL_mutex *)mHandle;
    SDL_UnlockMutex(h);
  }

  bool Mutex::tryEnter()
  {
    SDL_mutex *h = (SDL_mutex *)mHandle;
    return SDL_TryLockMutex(h) == 0;
  }

} /* namespace od */
