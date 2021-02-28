#include <hal/audio.h>
#include <hal/log.h>
#include <od/config.h>
#include <SDL2/SDL.h>

#define MAX_AUDIO_BUFFER_BYTES (AUDIO_NUM_CHANNELS * MAX_AUDIO_FRAME_LENGTH * sizeof(int))

static struct AudioLocals
{
  int buffer[CACHE_ALIGNED_SIZE(MAX_AUDIO_BUFFER_BYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  SDL_AudioSpec playSpec;
  SDL_AudioDeviceID playDev;
} local;

void playCallback(void *userdata,
                  Uint8 *stream,
                  int len)
{
  // Generate audio.
  Audio_callback(local.buffer);

  // Convert 24-bit to 32-bit and mix down to stereo.
  int *out = (int *)stream;
  for (int i = 0; i < FRAMELENGTH; i++)
  {
    // left channel = OUT1 + OUT3
    out[2 * i] = (local.buffer[4 * i] + local.buffer[4 * i + 2]) << 7;
    // right channel = OUT2 + OUT4
    out[2 * i + 1] = (local.buffer[4 * i + 1] + local.buffer[4 * i + 3]) << 7;
  }
}

void Audio_init()
{
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_zero(local);

  for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i)
  {
    logInfo("Audio driver %d: %s", i, SDL_GetAudioDriver(i));
  }

#if BUILDOPT_FORCE_ALSA
  if (SDL_AudioInit("alsa"))
  {
    logError("Failed to initialize alsa driver: %s", SDL_GetError());
    logInfo("Opening default driver...");
  }
#endif

  const char *driver_name = SDL_GetCurrentAudioDriver();

  if (driver_name)
  {
    logInfo("Audio subsystem initialized, driver = %s.", driver_name);
  }
  else
  {
    logError("Audio subsystem not initialized.");
  }
}

void Audio_restart(void)
{
  Audio_stop();
  SDL_Delay(200);
  Audio_start();
}

void Audio_start(void)
{
  SDL_AudioSpec want;
  SDL_zero(want);
  want.freq = globalConfig.sampleRate;
  want.format = AUDIO_S32;
  want.channels = 2;
  want.samples = globalConfig.frameLength;
  want.callback = playCallback;

  local.playDev = SDL_OpenAudioDevice(NULL, 0, &want, &local.playSpec, 0);
  if (local.playDev == 0)
  {
    logError("Failed to open audio: %s", SDL_GetError());
  }
  else
  {
    logInfo("Audio Specs: %dHz %dch", local.playSpec.freq, local.playSpec.channels);
    SDL_PauseAudioDevice(local.playDev, 0); /* start audio playing. */
  }
}

void Audio_stop(void)
{
  SDL_CloseAudioDevice(local.playDev);
}

uint32_t Audio_errorCount(void)
{
  return 0;
}

int Audio_getRate(void)
{
  return globalConfig.sampleRate;
}

void Audio_printErrorStatus(void)
{
  switch (SDL_GetAudioDeviceStatus(local.playDev))
  {
  case SDL_AUDIO_STOPPED:
    logInfo("audio stopped");
    break;
  case SDL_AUDIO_PLAYING:
    logInfo("audio playing");
    break;
  case SDL_AUDIO_PAUSED:
    logInfo("audio paused");
    break;
  default:
    logInfo("audio ???");
    break;
  }
}

int Audio_getLoad()
{
  return 0;
}
