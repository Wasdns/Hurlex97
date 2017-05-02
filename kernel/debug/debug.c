#include "debug.h"

// Hint - Debug Process
//
// 1.GRUB load kernel
// 2.information from ELF format executable file 
//   to multiboot_t structure
// 3.store (multiboot_t*)pointer in ebx register 
// 4.pass the ebx register to the kernel
//

// Hint - registers
//
// EAX：累加(Accumulator)寄存器，常用于函数返回值
// EBX：基址(Base)寄存器，以它为基址访问内存
// ECX：计数器(Counter)寄存器，常用作字符串和循环操作中的计数器
// EDX：数据(Data)寄存器，常用于乘除法和I/O指针
// ESI：源变址寄存器
// DSI：目的变址寄存器
// ESP：堆栈(Stack)指针寄存器，指向堆栈顶部
// EBP：基址指针寄存器，指向当前堆栈底部
// EIP：指令寄存器，指向下一条指令的地址
 
// Hint - panic
//
// 1.elf + address => function name
// 2.函数调用栈原理
//

static void print_stack_trace();
static elf_t kernel_elf;

void init_debug() {
    // 从 GRUB 提供的信息中获取到内核符号表和代码地址信息
    kernel_elf = elf_from_multiboot(glb_mboot_ptr);
}

void print_cur_status() {
    static int round = 0;
    uint16_t reg1, reg2, reg3, reg4;

    __asm__ __volatile__ (  "mov %%cs, %0;"
                            "mov %%ds, %1;"
                            "mov %%es, %2;"
                            "mov %%ss, %3;"
                            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

    // 打印当前的运行级别
    printk("%d: @ring %d\n", round, reg1 & 0x3);
    printk("%d:  cs = %x\n", round, reg1);
    printk("%d:  ds = %x\n", round, reg2);
    printk("%d:  es = %x\n", round, reg3);
    printk("%d:  ss = %x\n", round, reg4);
    ++round;
}

void panic(const char *msg) {
    printk("*** System panic: %s\n", msg);
    print_stack_trace();
    printk("***\n");
    
    // 致命错误发生后打印栈信息后停止在这里
    while(1);
}

void print_stack_trace() {
    uint32_t *ebp, *eip;

    __asm__ __volatile__("mov %%ebp, %0" : "=r" (ebp)); // get ebp value
    
    while (ebp) {
        eip = ebp + 1;
        
        // elf_lookup_symbol: address, elf => function name
        // you can overview its definition in elf.c
        printk("   [0x%x] %s\n", *eip, elf_lookup_symbol(*eip, &kernel_elf));
        
        ebp = (uint32_t*)*ebp;
    }
}