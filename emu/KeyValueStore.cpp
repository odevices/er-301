#include <emu/KeyValueStore.h>
#include <od/extras/Utils.h>
#define BUILDOPT_VERBOSE
#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <iostream>
#include <fstream>

namespace emu
{

  bool KeyValueStore::load(const std::string &fname)
  {
    std::ifstream f;
    f.open(fname);
    if (!f.is_open())
      return false;

    clear();

    int lineCount = 0;
    std::string line;
    std::vector<std::string> tokens;
    while (std::getline(f, line))
    {
      lineCount++;
      tokens.clear();
      od::split(line, ' ', tokens);
      if (tokens.size() == 0)
      {
        // skip empty lines
        continue;
      }
      else if (tokens.size() > 0 && od::startsWith(tokens[0], "#"))
      {
        // ignore commented lines
        continue;
      }
      else if (tokens.size() == 1)
      {
        logWarn("%s:%d: found only 1 token '%s'", fname.c_str(), lineCount, line.c_str());
        continue;
      }
      else if (tokens.size() == 2)
      {
        logDebug(1, "%s:%d: %s = %s",
                 fname.c_str(), lineCount, tokens[0].c_str(), tokens[1].c_str());
        (*this)[tokens[0]] = tokens[1];
      }
      else
      {
        logWarn("%s:%d: too many tokens '%s'", fname.c_str(), lineCount, line.c_str());
        continue;
      }
    }

    f.close();
    return true;
  }

  bool KeyValueStore::save(const std::string &fname)
  {
    std::ofstream f;
    f.open(fname);
    if (!f.is_open())
      return false;

    for (auto &kv : *this)
    {
      f << kv.first << " " << kv.second << '\n';
    }

    f.close();
    return true;
  }

  bool KeyValueStore::has(const std::string &key)
  {
    return find(key) != end();
  }

  int KeyValueStore::getInteger(const std::string &key, int defaultValue)
  {
    auto i = (*this).find(key);
    if (i == end())
    {
      return defaultValue;
    }
    else
    {
      return atoi(i->second.c_str());
    }
  }

  const std::string &KeyValueStore::get(const std::string &key, const std::string &defaultValue)
  {
    auto i = (*this).find(key);
    if (i == end())
    {
      return defaultValue;
    }
    else
    {
      return i->second;
    }
  }

  float KeyValueStore::getFloat(const std::string &key, float defaultValue)
  {
    auto i = (*this).find(key);
    if (i == end())
    {
      return defaultValue;
    }
    else
    {
      return atof(i->second.c_str());
    }
  }

  void KeyValueStore::setInteger(const std::string &key, int value)
  {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d", value);
    (*this)[key] = buffer;
  }

  void KeyValueStore::setFloat(const std::string &key, float value)
  {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%f", value);
    (*this)[key] = buffer;
  }

} // namespace emu
