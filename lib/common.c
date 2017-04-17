#include "common.h"

/*******************************************************
	
	Hint1 - 内嵌汇编，将C语言表达式指定为汇编指令的操作数
	__asm__(asm): 指定后面的代码为汇编代码
	__volatile__(volatile): 指定编译器不要优化代码

	Hint2 - inline
	内联函数, 通过避免被调用的开销来提高执行效率

 *******************************************************/

// Assembly Introduction: outb, inb, inw
// 
// "outb %1, %0" : : "dN" (port), "a" (value)
// outb [port] [value] == write value to the port
// 
// "inb %1, %0" : "=a" (ret) : "dN" (port)
// inb [result] [port] == read a byte from the port and 
// record it in the result
// 
// "inw %1, %0" : "=a" (ret) : "dN" (port)
// inw [result] [port] == read a word from the port and 
// record it in the result

inline void outb(uint16_t port, uint8_t value) { // 端口写一个字节
    __asm__ __volatile__("outb %1, %0" : : "dN" (port), "a" (value));
}

inline uint8_t inb(uint16_t port) { // 端口读一个字节
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

inline uint16_t inw(uint16_t port) { // 端口读一个字, w = word
    uint16_t ret;
    __asm__ __volatile__("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}