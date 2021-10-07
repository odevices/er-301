#include <hal/dir.h>
#include <hal/fileops.h>
#include <od/extras/Utils.h>

#include <cxxabi.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

namespace od
{

  std::string demangle(const char *mangled)
  {
    int status;
    std::unique_ptr<char[], void (*)(void *)> result(
        abi::__cxa_demangle(mangled, 0, 0, &status), std::free);
    return result.get() ? std::string(result.get()) : "error occurred";
  }

  std::string suggestTempFilename(const std::string &path)
  {
    int i = 0;
    std::string filename;

    do
    {
      std::ostringstream ss;
      ss << path << "/_" << std::setw(4) << std::setfill('0') << i << ".tmp";
      filename = ss.str();
      i++;
    } while (pathExists(filename.c_str()) && i < 9999);

    return filename;
  }

  std::vector<std::string> &split(const std::string &s, char delim,
                                  std::vector<std::string> &elems)
  {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim))
    {
      elems.push_back(item);
    }
    return elems;
  }

  std::vector<std::string> split(const std::string &s, char delim)
  {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
  }

  bool glob(const char *text, const char *pattern)
  {
    char t = *text;
    char p = *pattern;
    // convert to upper-case
    if (t <= 122 && t >= 97)
      t -= 32;
    if (p <= 122 && p >= 97)
      p -= 32;
#if 1
    if (p != '*')
    {
      if (t)
      {
        return ('?' == p || t == p) && glob(text + 1, pattern + 1);
      }
      else
      {
        return !p;
      }
    }
    else
    {
      return glob(text, pattern + 1) || (t && glob(text + 1, pattern));
    }
#else
    return p - 42 ? t ? (63 == p || t == p) && glob(text + 1, pattern + 1) : !p
                  : glob(text, pattern + 1) || (t && glob(text + 1, pattern));
#endif
  }

  void listFiles(const std::string &path, bool recursive,
                 std::vector<std::string> &result)
  {
    char *tmp;
    uint32_t attributes;
    std::string pname, fname;
    dir_t dir;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (recursive && (attributes & FILEOPS_DIR))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        listFiles(ss.str(), true, result);
      }
      else
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        result.emplace_back(ss.str());
      }
    }

    Dir_close(dir);
  }

  void listMatchingFiles(const std::string &path, bool recursive,
                         const std::string &pattern,
                         std::vector<std::string> &result)
  {
    char *tmp;
    uint32_t attributes;
    std::string pname, fname;
    dir_t dir;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (recursive && (attributes & FILEOPS_DIR))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        listMatchingFiles(ss.str(), true, pattern, result);
      }
      else if (glob(fname.c_str(), pattern.c_str()))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        result.emplace_back(ss.str());
      }
    }

    Dir_close(dir);
  }

  bool glob(const std::string &text, std::vector<std::string> &patterns)
  {
    for (const std::string &pattern : patterns)
    {
      if (glob(text.c_str(), pattern.c_str()))
      {
        return true;
      }
    }
    return false;
  }

  bool startsWith(const std::string &text, const std::string &token)
  {
    return text.compare(0, token.length(), token) == 0;
  }

  void listMatchingFiles(const std::string &path, bool recursive,
                         std::vector<std::string> &patterns,
                         std::vector<std::string> &result)
  {
    char *tmp;
    uint32_t attributes;
    std::string pname, fname;
    dir_t dir;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (recursive && (attributes & FILEOPS_DIR))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        listMatchingFiles(ss.str(), true, patterns, result);
      }
      else if (glob(fname.c_str(), patterns))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        result.emplace_back(ss.str());
      }
    }

    Dir_close(dir);
  }

  void listMatchingFiles(const std::string &path, bool recursive,
                         std::vector<std::string> &patterns,
                         std::set<std::string> &result)
  {
    char *tmp;
    uint32_t attributes;
    std::string pname, fname;
    dir_t dir;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (recursive && (attributes & FILEOPS_DIR))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        listMatchingFiles(ss.str(), true, patterns, result);
      }
      else if (glob(fname.c_str(), patterns))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        result.emplace(ss.str());
      }
    }

    Dir_close(dir);
  }

  int countMatchingFiles(const std::string &path, bool recursive,
                         std::vector<std::string> &patterns)
  {
    int count = 0;
    char *tmp;
    uint32_t attributes;
    std::string pname, fname;
    dir_t dir;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return 0;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (recursive && (attributes & FILEOPS_DIR))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        count += countMatchingFiles(ss.str(), true, patterns);
      }
      else if (glob(fname.c_str(), patterns))
      {
        count++;
      }
    }

    Dir_close(dir);
    return count;
  }

  std::string longestPath(const std::vector<std::string> &dirs, char separator)
  {
    std::vector<std::string>::const_iterator vsi = dirs.begin();
    if (vsi == dirs.end())
    {
      return std::string();
    }
    int maxCharactersCommon = vsi->length();
    std::string compareString = *vsi;
    for (vsi = dirs.begin() + 1; vsi != dirs.end(); vsi++)
    {
      std::pair<std::string::const_iterator, std::string::const_iterator> p =
          std::mismatch(compareString.begin(), compareString.end(),
                        vsi->begin());
      if ((p.first - compareString.begin()) < maxCharactersCommon)
        maxCharactersCommon = p.first - compareString.begin();
    }
    std::string::size_type found = compareString.rfind(separator,
                                                       maxCharactersCommon);
    return compareString.substr(0, found);
  }

  std::string join(const std::vector<std::string> &elements,
                   const char *const separator)
  {
    switch (elements.size())
    {
    case 0:
      return "";
    case 1:
      return elements[0];
    default:
      std::ostringstream os;
      std::copy(elements.begin(), elements.end() - 1,
                std::ostream_iterator<std::string>(os, separator));
      os << *elements.rbegin();
      return os.str();
    }
  }

} // namespace od
