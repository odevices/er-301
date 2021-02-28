#include <core/objects/pitch/ScaleQuantizer.h>
#include <od/extras/FileReader.h>
#include <hal/ops.h>
#include <hal/log.h>
#include <od/config.h>
#include <algorithm>
#include <math.h>

namespace od
{

  static char *skip_whitespace(char *ptr)
  {
    while (*ptr != 0 && isspace(*ptr))
    {
      ptr++;
    }
    return ptr;
  }

  static bool parse_scala_pitch(char *buffer, float *cents)
  {
    if (*buffer == 0)
    {
      return false;
    }

    // Cents or ratio?
    char *ptr = buffer;
    bool is_cents = false;
    while (*ptr != 0)
    {
      if (*ptr == '.')
      {
        is_cents = true;
        break;
      }
      ptr++;
    }

    if (is_cents)
    {
      *cents = atof(buffer);
    }
    else
    {
      int num = -1, den = -1;
      ptr = buffer;
      // parse numerator
      while (*ptr != 0 && !isspace(*ptr))
      {
        if (*ptr == '/')
        {
          *ptr = 0;
          num = atoi(buffer);
          *ptr = '/';
          ptr++;
          break;
        }
        ptr++;
      }
      if (num < 1)
      {
        return false;
      }
      // parse denominator
      den = atoi(ptr);
      if (den < 1)
      {
        return false;
      }
      float ratio = num / (float)den;
      *cents = 1200.0f * logf(ratio) / logf(2);
    }

    return true;
  }

  ScaleQuantizer::ScaleQuantizer()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mPreTranspose);
    addParameter(mPostTranspose);
    beginScale();
    addPitch(100);
    addPitch(200);
    addPitch(300);
    addPitch(400);
    addPitch(500);
    addPitch(600);
    addPitch(700);
    addPitch(800);
    addPitch(900);
    addPitch(1000);
    addPitch(1100);
    addPitch(1200);
    endScale();
  }

  ScaleQuantizer::~ScaleQuantizer()
  {
  }

  void ScaleQuantizer::beginScale()
  {
    mHaveScale = false;
    mPitchClasses.clear();
  }

  void ScaleQuantizer::endScale()
  {
    mPitchClasses.push_back(-1.0f);
    std::sort(mPitchClasses.begin(), mPitchClasses.end());
    mPitchBoundaries.clear();
    for (unsigned int i = 1; i < mPitchClasses.size(); i++)
    {
      mPitchBoundaries.push_back(
          0.5f * (mPitchClasses[i - 1] + mPitchClasses[i]));
    }
    mHaveScale = true;
  }

  void ScaleQuantizer::addPitch(float cents)
  {
    if (cents > 0.0f && cents < 1201.0f)
    {
      cents *= 1.0f / 1200.0f;
      mPitchClasses.push_back(cents);
      mPitchClasses.push_back(cents - 1.0f);
    }
  }

  bool ScaleQuantizer::loadScalaFile(const char *filename)
  {
    std::string line;
    FileReader reader;

    if (!reader.open(filename))
    {
      return false;
    }

    int state = 0;
    int size = 0;
    int degree = 1; // degree 0 is implicitly assigned to ratio 1/1
    float pitch;
    beginScale();
    while (reader.readLine(line))
    {
      char * ptr = &line[0];
      ptr = skip_whitespace(ptr);
      if (*ptr == '!')
      {
        // skip comments
        continue;
      }
      switch (state)
      {
      case 0: // looking for description line
        state = 1;
        break;
      case 1: // looking for size line
        size = atoi(ptr);
        if (size < 0 || size > 1024)
        {
          logError("Invalid size in scala file.");
          return false;
        }
        state = 2;
        break;
      case 2: // reading pitch lines
        if (parse_scala_pitch(ptr, &pitch))
        {
          addPitch(pitch);
          degree++;
        }
        else
        {
          logError("Error parsing scala pitch.");
          return false;
        }
        break;
      }

      if (state == 2 && degree > size + 1)
      {
        // Read 'size' number of pitches plus 1/1.
        break;
      }
    }

    if (state == 2)
    {
      endScale();
      return true;
    }
    else
      return false;
  }

  inline float ScaleQuantizer::quantize(float value)
  {
    float V = FULLSCALE_IN_VOLTS * value;
    int octave = (int)V;
    // Remove octaves
    float pitch = V - octave;
    // pitch is now in  (-1,1)
    // Find the first quantized pitch that is greater than the candidate pitch.
    std::vector<float>::iterator i = std::upper_bound(mPitchBoundaries.begin(),
                                                      mPitchBoundaries.end(),
                                                      pitch);
    pitch = mPitchClasses[std::distance(mPitchBoundaries.begin(), i)];
    // Add octaves back in
    V = pitch + octave;
    return V * (1.0f / FULLSCALE_IN_VOLTS);
  }

  void ScaleQuantizer::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float pre = mPreTranspose.value();
    float post = mPostTranspose.value();

    if (!mHaveScale)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = in[i] + pre + post;
      }
    }
    else if (mQuantizeAtSampleRate)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = quantize(in[i] + pre) + post;
      }
    }
    else
    {
      float pitch = quantize(in[0] + pre) + post;
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = pitch;
      }
    }

    mLastInput = in[FRAMELENGTH - 1] + pre + post;
    mLastOutput = out[FRAMELENGTH - 1];
  }

} /* namespace od */
