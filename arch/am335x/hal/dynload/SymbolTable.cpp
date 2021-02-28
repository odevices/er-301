#include "SymbolTable.h"

//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <od/extras/FileReader.h>
#include <od/extras/Utils.h>
#include <sstream>

namespace od
{

  SymbolTable::SymbolTable()
  {
  }

  SymbolTable::SymbolTable(const std::string &path)
  {
    mFilename = path;
    mLazyLoadEnabled = true;
  }

  SymbolTable::~SymbolTable()
  {
  }

  int SymbolTable::loadFromFile(const std::string &path)
  {
    FileReader reader;

    if (!reader.open(path))
    {
      logError("Unable to open: %s", path.c_str());
      return false;
    }

    mFilename = path;

    int found = 0;
    int lineNum = 0;
    for (std::string line; reader.readLine(line);)
    {
      lineNum++;
      std::stringstream ss(line);
      uintptr_t address;
      std::string token;
      // symbol address (in hex)
      ss >> token;
      if (ss.fail())
      {
        logWarn("%s:%d: address parse error.", path.c_str(), lineNum);
        continue;
      }
      address = strtoul(token.c_str(), 0, 16);
      // symbol type
      ss >> token;
      if (ss.fail())
      {
        logWarn("%s:%d: type parse error.", path.c_str(), lineNum);
        continue;
      }
      // symbol name
      ss >> token;
      if (ss.fail())
      {
        logWarn("%s:%d: name parse error.", path.c_str(), lineNum);
        continue;
      }
      // logDebug(1, "%s:%d: found %s @ %ul", path.c_str(), lineNum, token.c_str(), address);
      add(token, address);
      found++;
    }

    return found;
  }

  void SymbolTable::add(const std::string &symbolName, uintptr_t address)
  {
    mMap[symbolName] = address;
    logDebug(1, "SymbolTable(%s):add(%s,0x%x)", mFilename.c_str(), symbolName.c_str(), address);
  }

  void SymbolTable::clear()
  {
    mMap.clear();
  }

  uintptr_t SymbolTable::lookup(const std::string &symbolName)
  {
    if (mLazyLoadEnabled)
    {
      mLazyLoadEnabled = false;
      int count = loadFromFile(mFilename);
      logInfo("Lazy loaded %d symbols from %s.", count, mFilename.c_str());
    }

    auto i = mMap.find(symbolName);
    if (i != mMap.end())
    {
      return i->second;
    }

    return 0;
  }

  std::vector<std::string> SymbolTable::glob(const std::string &pattern)
  {
    std::vector<std::string> result;
    for (const auto &rec : mMap)
    {
      if (od::glob(rec.first.c_str(), pattern.c_str()))
      {
        result.push_back(rec.first);
      }
    }
    return result;
  }

} // namespace od