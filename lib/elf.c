#include "common.h"
#include "string.h"
#include "elf.h"

// GRUB => multiboot_t <=> ELF Information => ebx register
// kernel => ebx => get elf information
//
// GRUB在载入内核之后, 会读取ELF并把相关的信息组织成结构体放在
// multiboot_t结构, 并把结构体指针放在ebx寄存器里传递给内核

// 从 multiboot_t 结构获取ELF信息
elf_t elf_from_multiboot(multiboot_t *mb) {
    int i;
    elf_t elf;
    elf_section_header_t *sh = (elf_section_header_t *)mb->addr;

    uint32_t shstrtab = sh[mb->shndx].addr;
    for (i = 0; i < mb->num; i++) {
        const char *name = (const char *)(shstrtab + sh[i].name);
        // 在 GRUB 提供的 multiboot 信息中寻找
        // 内核 ELF 格式所提取的字符串表和符号表
        if (strcmp(name, ".strtab") == 0) {
            elf.strtab = (const char *)sh[i].addr;
            elf.strtabsz = sh[i].size;
        }
        if (strcmp(name, ".symtab") == 0) {
            elf.symtab = (elf_symbol_t*)sh[i].addr;
            elf.symtabsz = sh[i].size;
        }
    }

    return elf;
}

// 查看ELF的符号信息: address => function name 
const char *elf_lookup_symbol(uint32_t addr, elf_t *elf) { // based on the address
    int i;

    for (i = 0; i < (elf->symtabsz / sizeof(elf_symbol_t)); i++) {
        if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2) {
              continue;
        }
        // 通过函数调用地址查到函数的名字
        if ( (addr >= elf->symtab[i].value) && (addr < (elf->symtab[i].value + elf->symtab[i].size)) ) {
            return (const char *)((uint32_t)elf->strtab + elf->symtab[i].name);
        }
    }

    return NULL;
}
