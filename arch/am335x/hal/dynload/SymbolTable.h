#ifndef __SymbolTable_H__
#define __SymbolTable_H__

#include <od/extras/ReferenceCounted.h>
#include <stdint.h>
#include <string>
#include <map>
#include <vector>

namespace od
{

  class SymbolTable : public ReferenceCounted
  {
  public:
    SymbolTable();
    SymbolTable(const std::string &path);
    ~SymbolTable();

    int loadFromFile(const std::string &path);
    uintptr_t lookup(const std::string &name);
    bool contains(const std::string &name);
    void add(const std::string &name, uintptr_t address);
    bool update(const std::string &name, uintptr_t address);
    void clear();
    std::vector<std::string> glob(const std::string &pattern);

  protected:
    std::map<std::string, uintptr_t> mMap;

    std::string mFilename;
    int mCurrentLine = 0;
    bool mLazyLoadEnabled = false;
  };

} // namespace od

#endif // __SymbolTable_H__