#pragma once

#include <string>
#include <map>

namespace emu
{
  struct KeyValueStore : public std::map<std::string,std::string>
  {
    bool load(const std::string& filename);
    bool save(const std::string& filename);
    
    bool has(const std::string & key);
    int getInteger(const std::string & key, int defaultValue = 0);
    float getFloat(const std::string & key, float defaultValue = 0.0f);
    void setInteger(const std::string & key, int value);
    void setFloat(const std::string & key, float value);
  };
} // namespace emu
