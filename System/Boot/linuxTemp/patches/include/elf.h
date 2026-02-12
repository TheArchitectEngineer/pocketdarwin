#ifndef _POCKETDARWIN_HOST_ELF_WRAPPER_H
#define _POCKETDARWIN_HOST_ELF_WRAPPER_H

/*
 * Host compatibility header for macOS builds:
 * provide ELF types via Homebrew libelf's system-ELF definitions.
 */
#include <libelf/sys_elf.h>

#ifndef R_386_32
#define R_386_32 1
#endif
#ifndef R_386_PC32
#define R_386_PC32 2
#endif

#ifndef R_ARM_PC24
#define R_ARM_PC24 1
#endif
#ifndef R_ARM_ABS32
#define R_ARM_ABS32 2
#endif

#ifndef R_MIPS_32
#define R_MIPS_32 2
#endif
#ifndef R_MIPS_26
#define R_MIPS_26 4
#endif
#ifndef R_MIPS_HI16
#define R_MIPS_HI16 5
#endif
#ifndef R_MIPS_LO16
#define R_MIPS_LO16 6
#endif

#endif
