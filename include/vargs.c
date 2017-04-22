#ifndef INCLUDE_VARGS_H_
#define INCLUDE_VARGS_H_

/* Hint: 
	1.根据第一个参数的地址和类型，通过逐渐计算出以后每一个参数
	  的起始地址的方法取出每一个参数;
	2.函数调用的参数在内存里是连续的;
	3.函数调用是通过栈来传递参数;
	4.Assembly 调用约定cdecl - 参数入栈顺序: 从右往左.
*/

/*
	根据第一个参数的地址和类型，通过逐渐计算出以后每一个参数的起始地址的方法取出每一个参数。
*/

/* Build-in Functions */

typedef __builtin_va_list va_list;

#define va_start(ap, last)         (__builtin_va_start(ap, last))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap) 

/*  Details:

	#define  va_list              char *
	#define  va_start(p, first)   (p = (va_list)&first + sizeof(first))
	#define  va_arg(p, next)      (*(next*)((p += sizeof(next) ) - sizeof(next)))
	#define  va_end(p)            (p = (va_list)NULL) 
*/

#endif  // INCLUDE_VARGS_H_