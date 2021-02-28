#include <emu/KeyValueStore.h>
#include <stdio.h>
#include <string.h>

namespace emu
{

  bool KeyValueStore::load(const std::string &fname)
  {
    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp)
      return false;

    clear();

    char *buf = 0;
    size_t buflen = 0;

    while (getline(&buf, &buflen, fp) > 0)
    {
      char *nl = strchr(buf, '\n');
      if (nl == NULL)
        continue;
      *nl = 0;

      char *sep = strchr(buf, '=');
      if (sep == NULL)
        continue;
      *sep = 0;
      sep++;

      std::string s1 = buf;
      std::string s2 = sep;

      (*this)[s1] = s2;
    }

    if (buf)
      free(buf);

    fclose(fp);
    return true;
  }

  bool KeyValueStore::save(const std::string &fname)
  {
    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp)
      return false;

    for (auto &kv : *this)
    {
      fprintf(fp, "%s=%s\n", kv.first.c_str(), kv.second.c_str());
    }

    fclose(fp);
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
