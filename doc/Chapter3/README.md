## Chapter3

### Makefile和ld脚本的简单解释

1.gcc compile .c:

```
C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I include
```

- -m32 是生成32位代码，这样的话我们的开发环境也可以使用64位的Linux系统。

- -ggdb 和-gstabs+ 是添加相关的调试信息，调试对后期的排错很重要。

- -nostdinc 是不包含C语言的标准库里的头文件。

- -fno-builtin 是要求gcc不主动使用自己的内建函数，除非显式声明。

- -fno-stack-protector 是不使用栈保护等检测。

Hint: 后期要加上 `-std=C99` 指明C语言标准。

2.ld

```
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
```

- -T scripts/kernel.ld 是使用我们自己的链接器脚本。

- -m elf_i386 是生成i386平台下的ELF格式的可执行文件，这是Linux下的可执行文件格式。

- -nostdlib 不链接C语言的标准库。

3.Notes:

- 一个可执行的文件大致由代码段和数据段组成。

- 可执行文件有义务向操作系统提供代码段和数据段位置等信息(置于其组织头部区域)，以便操作系统正确找到它的代码段和数据段并加载执行。

- 不同OS对于executable文件的组织方式不一样，比如Linux的ELF(Executable and Linkable Format)。

- a)按照标准生成规范的Multiboot引导信息 b)使用标准的ELF格式 => 使GRUB把我们的内核正确的加载和执行。

### 启动镜像的制作

文件系统: 文件系统指的是操作系统用于明确磁盘或分区上的文件存储的方法和数据结构，即在磁盘上组织文件的方法。

### 内核的入口和初始化

- Makefile

- boot.s