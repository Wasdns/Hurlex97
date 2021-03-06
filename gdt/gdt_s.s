[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp+4]    ; 参数存入 eax 寄存器
    lgdt [eax]          ; 加载到 GDTR [修改原先GRUB设置]

    mov ax, 0x10        ; 加载我们的数据段描述符
    mov ds, ax          ; 更新所有可以更新的段寄存器
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;==========================================;
    ;   x86以后CPU所增加的指令流水线和高速缓存   
    ;   可能会在新的全局描述符表加载之后依旧保  
    ;   持之前的缓存，修改GDTR之后最安全的做法   
    ;   就是立即清空流水线和更新高速缓存。         
    ;==========================================;

    jmp 0x08:.flush     ; 远跳转， 0x08 是我们的代码段描述符
                        ; 远跳目的是清空流水线并串行化处理器
.flush:
    ret