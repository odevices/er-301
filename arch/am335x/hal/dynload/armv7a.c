#include "armv7a.h"
//#define BUILDOPT_VERBOSE
#include <hal/log.h>
#include <string.h>

bool verifyHeader(Elf32_Ehdr *header)
{
  static const char magic[] = {0x7f, 'E', 'L', 'F'};
  if (memcmp(header->e_ident, magic, sizeof(magic)) != 0)
  {
    logError("Bad magic.");
    return false;
  }

  if (header->e_type != ET_REL)
  {
    logError("Not a relocatable ELF.");
    return false;
  }

  //// Checks required by the ARMv7-A architecture.

  if (header->e_machine != EM_ARM)
  {
    logError("Target architecture (ARM) does not match.");
    return false;
  }

  if (header->e_ident[EI_CLASS] != ELFCLASS32)
  {
    logError("Target architecture (32-bit) does not match.");
    return false;
  }

  if (header->e_ident[EI_DATA] != ELFDATA2LSB)
  {
    logError("Target architecture (little-endian) does not match.");
    return false;
  }

  if ((header->e_entry & 3) != 0)
  {
    logError("Entry point is not aligned.");
    return false;
  }

  return true;
}

#if 0

#define CONFIG_THUMB2_KERNEL

#define ___opcode_identity32(x) ((uint32_t)(x))
#define __opcode_to_mem_arm(x) ___opcode_identity32(x)
#define __mem_to_opcode_arm(x) __opcode_to_mem_arm(x)

#define ___opcode_identity16(x) ((uint16_t)(x))
#define __opcode_to_mem_thumb16(x) ___opcode_identity16(x)
#define __mem_to_opcode_thumb16(x) __opcode_to_mem_thumb16(x)

bool relocate(const Elf32_Rel *rel, const Elf32_Sym *sym, uintptr_t loc, const char *symbolName)
{
  int32_t offset;
  uint32_t tmp;
#ifdef CONFIG_THUMB2_KERNEL
  uint32_t upper, lower, sign, j1, j2;
#endif
  unsigned int relotype = ELF32_R_TYPE(rel->r_info);

  switch (relotype)
  {
  case R_ARM_NONE:
    /* ignore */
    break;
  case R_ARM_REL32:
    *(uint32_t *)loc += sym->st_value - loc;
    break;
  case R_ARM_ABS32:
  case R_ARM_TARGET1:
    *(uint32_t *)loc += sym->st_value;
    break;

  case R_ARM_PC24:
  case R_ARM_CALL:
  case R_ARM_JUMP24:
    if (sym->st_value & 3)
    {
      logWarn("%s: interworking call (ARM -> Thumb)",
              symbolName);

      //logError("%s: unsupported interworking call (ARM -> Thumb)",
      //         symbolName);
      //return false;
    }

    offset = __mem_to_opcode_arm(*(uint32_t *)loc);
    offset = (offset & 0x00ffffff) << 2;
    if (offset & 0x02000000)
      offset -= 0x04000000;

    offset += sym->st_value - loc;
    if (offset <= (int32_t)0xfe000000 ||
        offset >= (int32_t)0x02000000)
    {
      logError("%s: relocation %u out of range (0x%08lx -> 0x%08lx)",
               symbolName,
               relotype, loc,
               sym->st_value);
      return false;
    }

    offset >>= 2;
    offset &= 0x00ffffff;

    *(uint32_t *)loc &= __opcode_to_mem_arm(0xff000000);
    *(uint32_t *)loc |= __opcode_to_mem_arm(offset);
    break;

  case R_ARM_V4BX:
    /* Preserve Rm and the condition code. Alter
			* other bits to re-code instruction as
			* MOV PC,Rm.
			*/
    *(uint32_t *)loc &= __opcode_to_mem_arm(0xf000000f);
    *(uint32_t *)loc |= __opcode_to_mem_arm(0x01a0f000);
    break;

  case R_ARM_PREL31:
    offset = *(uint32_t *)loc + sym->st_value - loc;
    *(uint32_t *)loc = offset & 0x7fffffff;
    break;

  case R_ARM_MOVW_ABS_NC:
  case R_ARM_MOVT_ABS:
    offset = tmp = __mem_to_opcode_arm(*(uint32_t *)loc);
    offset = ((offset & 0xf0000) >> 4) | (offset & 0xfff);
    offset = (offset ^ 0x8000) - 0x8000;

    offset += sym->st_value;
    if (relotype == R_ARM_MOVT_ABS)
      offset >>= 16;

    tmp &= 0xfff0f000;
    tmp |= ((offset & 0xf000) << 4) |
           (offset & 0x0fff);

    *(uint32_t *)loc = __opcode_to_mem_arm(tmp);
    break;

#ifdef CONFIG_THUMB2_KERNEL
  case R_ARM_THM_CALL:
  case R_ARM_THM_JUMP24:
    /*
			 * For function symbols, only Thumb addresses are
			 * allowed (no interworking).
			 *
			 * For non-function symbols, the destination
			 * has no specific ARM/Thumb disposition, so
			 * the branch is resolved under the assumption
			 * that interworking is not required.
			 */
    if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC &&
        !(sym->st_value & 1))
    {
      logError("%s: unsupported interworking call (Thumb -> ARM)",
               symbolName);
      return false;
    }

    upper = __mem_to_opcode_thumb16(*(uint16_t *)loc);
    lower = __mem_to_opcode_thumb16(*(uint16_t *)(loc + 2));

    /*
			 * 25 bit signed address range (Thumb-2 BL and B.W
			 * instructions):
			 *   S:I1:I2:imm10:imm11:0
			 * where:
			 *   S     = upper[10]   = offset[24]
			 *   I1    = ~(J1 ^ S)   = offset[23]
			 *   I2    = ~(J2 ^ S)   = offset[22]
			 *   imm10 = upper[9:0]  = offset[21:12]
			 *   imm11 = lower[10:0] = offset[11:1]
			 *   J1    = lower[13]
			 *   J2    = lower[11]
			 */
    sign = (upper >> 10) & 1;
    j1 = (lower >> 13) & 1;
    j2 = (lower >> 11) & 1;
    offset = (sign << 24) | ((~(j1 ^ sign) & 1) << 23) |
             ((~(j2 ^ sign) & 1) << 22) |
             ((upper & 0x03ff) << 12) |
             ((lower & 0x07ff) << 1);
    if (offset & 0x01000000)
      offset -= 0x02000000;
    offset += sym->st_value - loc;

    if (offset <= (int32_t)0xff000000 ||
        offset >= (int32_t)0x01000000)
    {
      logError("%s: relocation %u out of range (0x%08lx -> 0x%08lx)",
               symbolName,
               relotype, loc,
               sym->st_value);
      return false;
    }

    sign = (offset >> 24) & 1;
    j1 = sign ^ (~(offset >> 23) & 1);
    j2 = sign ^ (~(offset >> 22) & 1);
    upper = (uint16_t)((upper & 0xf800) | (sign << 10) |
                       ((offset >> 12) & 0x03ff));
    lower = (uint16_t)((lower & 0xd000) |
                       (j1 << 13) | (j2 << 11) |
                       ((offset >> 1) & 0x07ff));

    *(uint16_t *)loc = __opcode_to_mem_thumb16(upper);
    *(uint16_t *)(loc + 2) = __opcode_to_mem_thumb16(lower);
    break;

  case R_ARM_THM_MOVW_ABS_NC:
  case R_ARM_THM_MOVT_ABS:
    upper = __mem_to_opcode_thumb16(*(uint16_t *)loc);
    lower = __mem_to_opcode_thumb16(*(uint16_t *)(loc + 2));

    /*
			 * MOVT/MOVW instructions encoding in Thumb-2:
			 *
			 * i	= upper[10]
			 * imm4	= upper[3:0]
			 * imm3	= lower[14:12]
			 * imm8	= lower[7:0]
			 *
			 * imm16 = imm4:i:imm3:imm8
			 */
    offset = ((upper & 0x000f) << 12) |
             ((upper & 0x0400) << 1) |
             ((lower & 0x7000) >> 4) | (lower & 0x00ff);
    offset = (offset ^ 0x8000) - 0x8000;
    offset += sym->st_value;

    if (relotype == R_ARM_THM_MOVT_ABS)
      offset >>= 16;

    upper = (uint16_t)((upper & 0xfbf0) |
                       ((offset & 0xf000) >> 12) |
                       ((offset & 0x0800) >> 1));
    lower = (uint16_t)((lower & 0x8f00) |
                       ((offset & 0x0700) << 4) |
                       (offset & 0x00ff));
    *(uint16_t *)loc = __opcode_to_mem_thumb16(upper);
    *(uint16_t *)(loc + 2) = __opcode_to_mem_thumb16(lower);
    break;
#endif

  default:
    logError("%s: unknown relocation: %u",
             symbolName, relotype);
    return false;
  }

  return true;
}

#else
bool relocate(const Elf32_Rel *rel, const Elf32_Sym *sym, uintptr_t addr, const char *symbolName)
{
  int32_t offset;
  unsigned int relotype;

  /* All relocations except R_ARM_V4BX depend upon having valid symbol
   * information.
   */

  relotype = ELF32_R_TYPE(rel->r_info);
  if (sym == NULL && relotype != R_ARM_NONE && relotype != R_ARM_V4BX)
  {
    return false;
  }

  /* Handle the relocation by relocation type */

  switch (relotype)
  {
  case R_ARM_NONE:
  {
    /* No relocation */
  }
  break;

  case R_ARM_PC24:
  case R_ARM_CALL:
  case R_ARM_JUMP24:
  {
    logDebug(1, "%s: CALL [%d] addr 0x%08lx [0x%08lx] to st_value=0x%08lx",
             symbolName, relotype, (long)addr, (long)(*(uint32_t *)addr), (long)sym->st_value);

    if (sym->st_value & 3)
    {
      logError("%s: Unsupported interworking call (ARM -> Thumb)", symbolName);
      return false;
    }

    offset = (*(uint32_t *)addr & 0x00ffffff) << 2;
    if (offset & 0x02000000)
    {
      offset -= 0x04000000;
    }

    offset += sym->st_value - addr;

    if (offset & 3 || offset < (int32_t)0xfe000000 || offset >= (int32_t)0x02000000)
    {
      logError("%s: CALL [%d] relocation out of range, offset=0x%08lx",
               symbolName, relotype, offset);
      return false;
    }

    offset >>= 2;

    *(uint32_t *)addr &= 0xff000000;
    *(uint32_t *)addr |= offset & 0x00ffffff;
    logDebug(1, "%s:: CALL [%d] addr 0x%08lx [0x%08lx] to st_value=0x%08lx",
             symbolName, relotype, (long)addr, (long)(*(uint32_t *)addr), (long)sym->st_value);
  }
  break;

  case R_ARM_ABS32:
  case R_ARM_TARGET1: /* New ABI:  TARGET1 always treated as ABS32 */
  {
    logDebug(1, "%s: ABS32 addr=0x%08lx [0x%08lx] st_value=0x%08lx",
             symbolName, (long)addr, (long)(*(uint32_t *)addr), (long)sym->st_value);
    *(uint32_t *)addr += sym->st_value;
  }
  break;

  case R_ARM_V4BX:
  {
    logDebug(1, "%s: V4BX addr=0x%08lx [0x%08lx]",
             symbolName, (long)addr, (long)(*(uint32_t *)addr));
    /* Preserve only Rm and the condition code */
    *(uint32_t *)addr &= 0xf000000f;
    /* Change instruction to 'mov pc, Rm' */
    *(uint32_t *)addr |= 0x01a0f000;
  }
  break;

  case R_ARM_PREL31:
  {
    logDebug(1, "%s: PREL31 addr=0x%08lx [0x%08lx] st_value=0x%08lx",
             symbolName, (long)addr, (long)(*(uint32_t *)addr), (long)sym->st_value);
    offset = *(uint32_t *)addr + sym->st_value - addr;
    *(uint32_t *)addr = offset & 0x7fffffff;
  }
  break;

  case R_ARM_MOVW_ABS_NC:
  case R_ARM_MOVT_ABS:
  {
    logDebug(1, "%s: MOVx_ABS [%d] addr=0x%08lx [0x%08lx] st_value=0x%08lx",
             symbolName, relotype, (long)addr, (long)(*(uint32_t *)addr),
             (long)sym->st_value);
    offset = *(uint32_t *)addr;
    offset = ((offset & 0xf0000) >> 4) | (offset & 0xfff);
    offset += sym->st_value;
    if (relotype == R_ARM_MOVT_ABS)
    {
      offset >>= 16;
    }
    *(uint32_t *)addr &= 0xfff0f000;
    *(uint32_t *)addr |= ((offset & 0xf000) << 4) | (offset & 0x0fff);
  }
  break;

  default:
    logError("%s: Unsupported relocation type: %d", symbolName, relotype);
    return false;
  }

  return true;
}
#endif