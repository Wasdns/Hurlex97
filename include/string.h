#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include "types.h"

// C语言标准库函数的实现

// Hint - memcpy
// 从源src所指的内存地址的起始位置开始拷贝n个字节
// 到目标dest所指的内存地址的起始位置中。 
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);

// Hint - memset
// 将dest所指向的某一块内存中的前len个字节的内容
// 全部设置为指定的val值
void memset(void *dest, uint8_t val, uint32_t len);

// Hint - bzero
// 置字节字符串前len个字节为零且包括‘\0’
void bzero(void *dest, uint32_t len);

// Hint - strcmp
// 比较两个字符串设这两个字符串为str1，str2
// 若str1==str2，则返回零；若str1str2，则返回正数。
int strcmp(const char *str1, const char *str2);

// Hint - strcpy
// 把从src地址开始且含有'\0'结束符的字符串复制到以dest开始的地址空间。
char *strcpy(char *dest, const char *src);

// Hint - strcat
// 将两个char类型连接
char *strcat(char *dest, const char *src);

// Hint - strlen
// 返回字符串长度
int strlen(const char *src);

#endif  // INCLUDE_STRING_H_