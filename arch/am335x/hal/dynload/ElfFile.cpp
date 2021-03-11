
#include "ElfFile.h"
#include <od/extras/FileReader.h>
#define BUILDOPT_VERBOSE
#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/dma.h>
#include <hal/heap.h>
#include <hal/constants.h>
#include <vector>
#include <string.h>
#include "elf.h"
#include "armv7a.h"
#include "reflect.h"

#define CONFIG_ELF_ALIGN_LOG2 2
#define ELF_ALIGN_MASK ((1 << CONFIG_ELF_ALIGN_LOG2) - 1)
#define ELF_ALIGNUP(a) (((unsigned long)(a) + ELF_ALIGN_MASK) & ~ELF_ALIGN_MASK)
#define ELF_ALIGNDOWN(a) ((unsigned long)(a) & ~ELF_ALIGN_MASK)

namespace od
{

  ElfFile::ElfFile()
  {
  }

  ElfFile::~ElfFile()
  {
    freeTextSpace();
    freeDataSpace();
  }

  bool ElfFile::allocateTextSpace(size_t bytes)
  {
    freeTextSpace();
    if (bytes > 0)
    {
      logDebug(1, "%s: Allocating %d bytes for text/code.", mFilename.c_str(), bytes);
      mpTextSpace = (uint8_t *)Heap_memalign(CACHELINE_SIZE_MAX, bytes);
      if (mpTextSpace)
      {
        bzero(mpTextSpace, bytes);
        mTextSize = bytes;
        return true;
      }
      else
      {
        logError("%s: Failed to allocate %d bytes of text/code memory.", mFilename.c_str(), bytes);
        return false;
      }
    }
    return true;
  }

  bool ElfFile::allocateDataSpace(size_t bytes)
  {
    freeDataSpace();
    if (bytes > 0)
    {
      logDebug(1, "%s: Allocating %d bytes for data.", mFilename.c_str(), bytes);
      mpDataSpace = (uint8_t *)Heap_memalign(CACHELINE_SIZE_MAX, bytes);
      if (mpDataSpace)
      {
        bzero(mpDataSpace, bytes);
        mDataSize = bytes;
        return true;
      }
      else
      {
        logError("%s: Failed to allocate %d bytes of data memory.", mFilename.c_str(), bytes);
        return false;
      }
    }
    return true;
  }

  void ElfFile::freeTextSpace()
  {
    if (mpTextSpace)
    {
      logDebug(1, "%s: freeing %d bytes of text/code memory.", mFilename.c_str(), mTextSize);
      Heap_free(mpTextSpace);
      mpTextSpace = 0;
      mTextSize = 0;
    }
  }

  void ElfFile::freeDataSpace()
  {
    if (mpDataSpace)
    {
      logDebug(1, "%s: freeing %d bytes of data memory.", mFilename.c_str(), mDataSize);
      Heap_free(mpDataSpace);
      mpDataSpace = 0;
      mDataSize = 0;
    }
  }

  bool ElfFile::load(const std::string &filename)
  {
    FileReader reader;
    if (!reader.open(filename))
    {
      logError("Failed to open %s.", filename.c_str());
      return false;
    }
    mFilename = filename;

    // First read and verify the file header.
    Elf32_Ehdr elfHeader;
    if (reader.readBytes(&elfHeader, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr))
    {
      logError("Could not read header.");
      return false;
    }
    else if (!verifyHeader(&elfHeader))
    {
      logError("Header verification failed.");
      return false;
    }

    // Load section headers.
    if (elfHeader.e_shnum < 1)
    {
      logError("No section headers to load.");
      return false;
    }

    size_t shdrsize = (size_t)elfHeader.e_shentsize * (size_t)elfHeader.e_shnum;
    if (elfHeader.e_shoff + shdrsize > reader.getSizeInBytes())
    {
      logError("File is not large enough to contain the section header table.");
      return false;
    }

    std::vector<Elf32_Shdr> sectionHeaders;
    sectionHeaders.resize(elfHeader.e_shnum);
    if (reader.readBytes(sectionHeaders.data(), shdrsize, elfHeader.e_shoff) != shdrsize)
    {
      logError("Could not read section headers.");
      return false;
    }

    // Determine size and allocate memory for sections.
    size_t textsize = 0;
    size_t datasize = 0;
    for (int i = 0; i < elfHeader.e_shnum; i++)
    {
      Elf32_Shdr &shdr = sectionHeaders[i];
      if ((shdr.sh_flags & SHF_ALLOC) != 0)
      {
        if ((shdr.sh_flags & SHF_WRITE) != 0)
        {
          datasize += ELF_ALIGNUP(shdr.sh_size);
        }
        else
        {
          textsize += ELF_ALIGNUP(shdr.sh_size);
        }
      }
    }

    if (!allocateTextSpace(textsize))
    {
      return false;
    }
    else if (!allocateDataSpace(datasize))
    {
      return false;
    }

    // Load the sections that are needed for in-memory execution.
    uint8_t *text = mpTextSpace;
    uint8_t *data = mpDataSpace;
    uint8_t **pptr;

    for (int i = 0; i < elfHeader.e_shnum; i++)
    {
      Elf32_Shdr &shdr = sectionHeaders[i];

      if ((shdr.sh_flags & SHF_ALLOC) == 0)
      {
        // Section requires memory during execution.
        continue;
      }

      if ((shdr.sh_flags & SHF_WRITE) != 0)
      {
        // Section address space is write-able.
        pptr = &data;
      }
      else
      {
        // Section address space is read-only.
        pptr = &text;
      }

      if (shdr.sh_type != SHT_NOBITS)
      {
        // Section has data.
        logDebug(11, "Loading section #%d into memory, reading %d bytes @ %d.", i, shdr.sh_size, shdr.sh_offset);
        if (reader.readBytes(*pptr, shdr.sh_size, shdr.sh_offset) != shdr.sh_size)
        {
          logError("Failed to read section #%d data.", i);
          return false;
        }
      }
      else
      {
        // Section has no data, so zero it out.
        memset(*pptr, 0, shdr.sh_size);
      }

      // Note the section in-memory location.
      logDebug(11, "Section %d address 0x%08x->0x%08x", i,
               (unsigned long)shdr.sh_addr, (unsigned long)*pptr);
      shdr.sh_addr = (uintptr_t)*pptr;

      // Increment the memory pointer by aligned section size.
      *pptr += ELF_ALIGNUP(shdr.sh_size);
    }

    // Load the symbol and string tables.
    std::vector<Elf32_Sym> symbolTable;
    std::vector<char> stringTable;
    for (int i = 1; i < elfHeader.e_shnum; i++)
    {
      Elf32_Shdr &shdr = sectionHeaders[i];
      if (shdr.sh_type == SHT_SYMTAB)
      {
        if (shdr.sh_size == 0)
        {
          logError("The symbol table has no data.");
          return false;
        }
        symbolTable.resize(shdr.sh_size / sizeof(Elf32_Sym));
        logDebug(1, "Loading symbol table into memory, reading %d bytes.", shdr.sh_size);
        if (reader.readBytes(symbolTable.data(), shdr.sh_size, shdr.sh_offset) != shdr.sh_size)
        {
          logError("Failed to read the symbol table.");
          return false;
        }

        Elf32_Shdr &link = sectionHeaders[shdr.sh_link];
        if (link.sh_size > 0)
        {
          stringTable.resize(link.sh_size);
          logDebug(1, "Loading string table into memory, reading %d bytes.", link.sh_size);
          if (reader.readBytes(stringTable.data(), link.sh_size, link.sh_offset) != link.sh_size)
          {
            logError("Failed to read the string table.");
            return false;
          }
        }
        else
        {
          logWarn("The string table is empty.");
          stringTable.resize(1);
          stringTable[0] = '\0';
        }

        break;
      }
    }

    if (symbolTable.size() == 0)
    {
      logError("The symbol table is empty.");
      return false;
    }

    // Resolve all symbols in the symbol table.
    logDebug(1, "Resolving undefined symbols...");
    int unresolved = 0;
    for (Elf32_Sym &sym : symbolTable)
    {
      // Resolve symbol value/address.
      switch (sym.st_shndx)
      {
      case SHN_COMMON:
        logError("Common symbols are not supported. Re-compile with -fno-common.");
        return false;
      case SHN_ABS:
        // Already has correct value.
        break;
      case SHN_UNDEF:
        if (sym.st_name == 0)
        {
          // This undefined symbol has no name.  Do nothing.
        }
        else if (sym.st_name < stringTable.size())
        {
          const char *symbolName = stringTable.data() + sym.st_name;
          // Look for this symbol in the environment.
          uintptr_t address = (uintptr_t)reflect_query_symbol(symbolName);
          if (address)
          {
            sym.st_value += address;
            logDebug(11, "%s resolved to 0x%08x.", symbolName, sym.st_value);
          }
          else
          {
            logWarn("Unresolved symbol: %s", symbolName);
            unresolved++;
          }
        }
        else
        {
          logError("Symbol %d name is not in the string table.", sym.st_name);
          return false;
        }
      default:
      {
        sym.st_value += sectionHeaders[sym.st_shndx].sh_addr;
        if (ELF32_ST_BIND(sym.st_info) == STB_GLOBAL && ELF32_ST_TYPE(sym.st_info) == STT_FUNC)
        {
          const char *symbolName = stringTable.data() + sym.st_name;
          logDebug(11, "export %s @ 0x%08x", symbolName, sym.st_value);
          mSymbols.add(symbolName, sym.st_value);
        }
        break;
      }
      }
    }

    if (unresolved > 0)
    {
      logError("Aborting ELF load due to %d unresolved symbol(s).", unresolved);
      return false;
    }

    // Perform relocations on each section.
    logDebug(1, "Performing relocations...");
    int failedRelocations = 0;
    for (int i = 1; i < elfHeader.e_shnum; i++)
    {
      Elf32_Shdr &shdr = sectionHeaders[i];
      if (shdr.sh_info >= elfHeader.e_shnum)
      {
        continue;
      }
      else if ((sectionHeaders[shdr.sh_info].sh_flags & SHF_ALLOC) == 0)
      {
        // Section was not loaded into memory.
        continue;
      }

      // The header for the section to be relocated.
      Elf32_Shdr &dstsec = sectionHeaders[shdr.sh_info];

      if (shdr.sh_type == SHT_REL)
      {
        int nrels = shdr.sh_size / sizeof(Elf32_Rel);
        if (nrels > 0)
        {
          // Read in the relocation table.
          std::vector<Elf32_Rel> rels(nrels);
          if (reader.readBytes(rels.data(), shdr.sh_size, shdr.sh_offset) != shdr.sh_size)
          {
            logError("Failed to read relocation table.");
            return false;
          }

          for (int j = 0; j < nrels; j++)
          {
            Elf32_Rel *rel = &rels[j];
            size_t symidx = ELF32_R_SYM(rel->r_info);
            if (symidx >= symbolTable.size())
            {
              logError("Symbol %d referenced in relocation is not in the symbol table.", symidx);
              return false;
            }
            Elf32_Sym *sym = &symbolTable[symidx];
            const char *symbolName = stringTable.data() + sym->st_name;
            if (sym->st_shndx == SHN_UNDEF && sym->st_name == 0)
            {
              sym = NULL;
            }

            if (rel->r_offset < 0 || rel->r_offset > dstsec.sh_size - sizeof(uint32_t))
            {
              logError("%s: Relocation address is out of range.", symbolName);
              return false;
            }

            if (!relocate(rel, sym, dstsec.sh_addr + rel->r_offset, symbolName))
            {
              logError("%s: Relocation failed.", symbolName);
              failedRelocations++;
            }
          }
        }
      }
      else if (shdr.sh_type == SHT_RELA)
      {
        logError("SHT_RELA relocation type is not supported.");
        return false;
      }
    }

    Edma3_CacheFlush((uint32_t)mpDataSpace, mDataSize);
    Edma3_CacheFlush((uint32_t)mpTextSpace, mTextSize);
    Edma3_CacheInvalidate((uint32_t)mpDataSpace, mDataSize);
    Edma3_CacheInvalidate((uint32_t)mpTextSpace, mTextSize);

    mFilename = filename;
    return failedRelocations == 0;
  }

  bool ElfFile::unload()
  {
    freeTextSpace();
    freeDataSpace();
    return true;
  }

  void *ElfFile::lookup(const std::string &symbol)
  {
    logDebug(1, "%s", symbol.c_str());
    return (void *)mSymbols.lookup(symbol.c_str());
  }

  std::vector<std::string> ElfFile::glob(const std::string &pattern)
  {
    return mSymbols.glob(pattern);
  }

} // namespace od