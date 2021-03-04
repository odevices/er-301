#include <od/config.h>
#include <hal/card.h>
#include <hal/log.h>
#include <hal/constants.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 128
#define TOKEN_SRATE "SAMPLERATE"
#define TOKEN_FLEN "FRAMELENGTH"

ConfigData globalConfig;

static void update()
{
  logInfo("%s = %d Hz", TOKEN_SRATE, globalConfig.sampleRate);
  logInfo("%s = %d samples", TOKEN_FLEN, globalConfig.frameLength);
  globalConfig.samplePeriod = 1.0f / globalConfig.sampleRate;
  globalConfig.frameRate = globalConfig.sampleRate / globalConfig.frameLength;
  globalConfig.framePeriod = globalConfig.frameLength * globalConfig.samplePeriod;
}

static char *skipWhitespace(char *p)
{
  unsigned char c = *p;
  while (c && isspace(c))
  {
    p++;
    c = *p;
  }
  return p;
}

void Config_init(const char *filename, const char * xRoot, const char * rearRoot, const char * frontRoot)
{
  globalConfig.frameLength = 128;
  globalConfig.sampleRate = 48000;
  globalConfig.filename = filename;
  globalConfig.frontRoot = frontRoot;
  globalConfig.rearRoot = rearRoot;
  globalConfig.xRoot = xRoot;
  if (Card_mount(0))
  {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    char *p;
    bool foundSampleRate = false;
    bool foundFrameLength = false;

    fp = fopen(globalConfig.filename, "r");
    if (fp == NULL)
    {
      logWarn("%s does not exist. Using default firmware configuration.", globalConfig.filename);
    }
    else
    {
      while (fgets(line, MAX_LINE_LENGTH, fp))
      {
        p = skipWhitespace(line);
        if (*p == 0 || *p == '#')
        {
          // Skip this line.
        }
        else if (strncmp(p, TOKEN_SRATE, strlen(TOKEN_SRATE)) == 0)
        {
          p += strlen(TOKEN_SRATE);
          p = skipWhitespace(p);
          if (*p)
          {
            int value = atoi(p);
            if (value == 48000 || value == 96000)
            {
              globalConfig.sampleRate = value;
              foundSampleRate = true;
            }
          }
        }
        else if (strncmp(p, TOKEN_FLEN, strlen(TOKEN_FLEN)) == 0)
        {
          p += strlen(TOKEN_FLEN);
          p = skipWhitespace(p);
          if (*p)
          {
            int value = atoi(p);
            if (value > 1 && value <= MAX_AUDIO_FRAME_LENGTH)
            {
              globalConfig.frameLength = value;
              foundFrameLength = true;
            }
          }
        }
      }
      fclose(fp);
      if (!foundSampleRate)
      {
        logWarn("%s not found in %s.", TOKEN_SRATE, globalConfig.filename);
      }
      if (!foundFrameLength)
      {
        logWarn("%s not found in %s.", TOKEN_FLEN, globalConfig.filename);
      }
    }
  }
  update();
}

bool Config_store(int sampleRate, int frameLength)
{
  FILE *fp;

  fp = fopen(globalConfig.filename, "w");
  if (fp == NULL)
  {
    logError("Failed to write configuration to %s.", globalConfig.filename);
    return false;
  }

  fprintf(fp, "%s\t%d\n", TOKEN_SRATE, sampleRate);
  fprintf(fp, "%s\t%d\n", TOKEN_FLEN, frameLength);
  fclose(fp);

  return true;
}
