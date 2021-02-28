#ifndef __ElfFile_H__
#define __ElfFile_H__

#include <od/extras/ReferenceCounted.h>
#include <hal/dynload/SymbolTable.h>
#include <string>
#include <vector>

namespace od
{

  class ElfFile : public ReferenceCounted
  {
  public:
    ElfFile();
    ~ElfFile();

    bool load(const std::string &filename);
    bool unload();
    void *lookup(const std::string &symbol);
    std::vector<std::string> glob(const std::string &pattern);
    const std::string &path()
    {
      return mFilename;
    }

  protected:
    std::string mFilename;
    SymbolTable mSymbols;
    uint8_t *mpTextSpace = 0;
    uint8_t *mpDataSpace = 0;
    size_t mTextSize = 0;
    size_t mDataSize = 0;

    bool allocateTextSpace(size_t bytes);
    bool allocateDataSpace(size_t bytes);
    void freeTextSpace();
    void freeDataSpace();
  };

} // namespace od

#endif // __ElfFile_H__