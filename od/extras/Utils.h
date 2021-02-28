#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace od
{

  typedef std::map<std::string, std::string> StringMap;

  std::string demangle(const char *mangled);

  std::string suggestTempFilename(const std::string &path);

  bool fileExists(const std::string &filename);

  std::vector<std::string> &split(const std::string &s, char delim,
                                  std::vector<std::string> &elems);

  std::vector<std::string> split(const std::string &s, char delim);

  std::string join(const std::vector<std::string> &elements,
                   const char *const separator);

  void listFiles(const std::string &path, bool recursive,
                 std::vector<std::string> &result);

  void listMatchingFiles(const std::string &path, bool recursive,
                         const std::string &pattern,
                         std::vector<std::string> &result);
  void listMatchingFiles(const std::string &path, bool recursive,
                         std::vector<std::string> &globs,
                         std::vector<std::string> &result);

  void listMatchingFiles(const std::string &path, bool recursive,
                         std::vector<std::string> &globs,
                         std::set<std::string> &result);                         

  int countMatchingFiles(const std::string &path, bool recursive,
                         std::vector<std::string> &globs,
                         std::set<std::string> &result);   

  bool glob(const char *text, const char *pattern);
  bool glob(const std::string &text, std::vector<std::string> &patterns);

  bool startsWith(const std::string &text, const std::string &token);

  std::string longestPath(const std::vector<std::string> &dirs, char separator =
                                                                    '/');

  // trim from start (in place)
  static inline void ltrim(std::string &s)
  {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))));
  }

  // trim from end (in place)
  static inline void rtrim(std::string &s)
  {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace)))
                .base(),
            s.end());
  }

  // trim from both ends (in place)
  static inline void trim(std::string &s)
  {
    ltrim(s);
    rtrim(s);
  }

  // trim from start (copying)
  static inline std::string ltrim_copy(std::string s)
  {
    ltrim(s);
    return s;
  }

  // trim from end (copying)
  static inline std::string rtrim_copy(std::string s)
  {
    rtrim(s);
    return s;
  }

  // trim from both ends (copying)
  static inline std::string trim_copy(std::string s)
  {
    trim(s);
    return s;
  }

#if 1
  static inline int fastRound(double d)
  {
    union Cast
    {
      double d;
      long l;
    };
    volatile Cast c;
    c.d = d + 6755399441055744.0;
    return c.l;
  }
#else
  static inline int fastRound(float f)
  {
    return (int)(f + 0.5f);
  }
#endif

  static inline int fastTruncate(double d)
  {
    union Cast
    {
      double d;
      long l;
    };
    volatile Cast c;
    c.d = d + 6755399441055743.5;
    return c.l;
  }

} // namespace od
