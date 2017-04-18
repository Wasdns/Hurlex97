#include <console.h>

/* Hint Begin */

// HintA - VGA: 
// VGA(Video Graphics Array，视频图形阵列)是使用模拟信号的一种视频传输标准，
// 内核可以通过它来控制屏幕上字符或者图形的显示。
// 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)
// 作为屏幕上字符显示的缓冲区，若要改变屏幕上字符的显示，只需要修改这块内存就好了。

// HintB - *(video_memory+i)
//
// *(video_memory+i) <=> 
// background color(4 bits) + front color(4 bits) + attribute_type(1 byte)
//
// background color      front color	
// 0   1   2   3   4   5   6   7   8
// +---+---+---+---+---+---+---+---+
// | K | R | G | B | I | R | G | B |
// +---+---+---+---+---+---+---+---+

// HintC - attribute type
// Definition: uint8_t attribute_byte;

/* Hint End */

/* Definitions Begin */

// VGA的显示缓冲的起点是 0xB8000, video_memory => 屏幕数组, static => 限定作用域
static uint16_t *video_memory = (uint16_t *)0xB8000;

// 屏幕"光标"的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

/* Definitions End */

/* Static Functions Begin */

// 移动光标
static void move_cursor()
{
    // 屏幕是 80 字节宽
    uint16_t cursorLocation = cursor_y * 80 + cursor_x;
    
    // 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置的高8位与低8位。

    // outb: 端口写一个字节，0x3D4 <=> VGA, 0x3D5 <=> Registers
    // Definition: void outb(uint16_t port, uint8_t value);
    
    outb(0x3D4, 14);                    // 告诉 VGA 我们要设置光标的高字节
    outb(0x3D5, cursorLocation >> 8);   // 发送高 8 位
    outb(0x3D4, 15);                    // 告诉 VGA 我们要设置光标的低字节
    outb(0x3D5, cursorLocation);        // 发送低 8 位
}

// 滚动屏幕: 将后24行的数据全部向上挪动一行, 最后一行清空
static void scroll()
{
    // attribute_byte 被构造出一个黑底白字的描述格式
    uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
    uint16_t blank = 0x20 | (attribute_byte << 8);  // space 是 0x20

    // cursor_y 到 25 的时候，就该换行了
    if (cursor_y >= 25) {
        int i;
        // 用下一行的数据覆盖本行数据
        for (i = 0 * 80; i < 24 * 80; i++) {
              video_memory[i] = video_memory[i+80];
        }

        // 最后的一行数据填充空格，不显示任何字符
        for (i = 24 * 80; i < 25 * 80; i++) {
              video_memory[i] = blank;
        }
        
        // 向上移动了一行，所以 cursor_y 现在是 24
        cursor_y = 24;
    }
}

/* Static Functions End */

/* Functions Begin */

// 清空 <=> 用空格键覆盖整个区域
void console_clear()
{
    uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);	// blank
    uint16_t blank = 0x20 | (attribute_byte << 8);

    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = blank;
    }

    cursor_x = 0; 
    cursor_y = 0;
    move_cursor();
}

// 屏幕输出一个字符(带颜色):
// (1) write information into video_memory (color_info + control_info)
// (2) screen scroll (3) move cursor_x and cursor_y (change register values) 
void console_putc_color(char c, real_color_t back, real_color_t fore) {
    uint8_t back_color = (uint8_t)back;	// background
    uint8_t fore_color = (uint8_t)fore; // front

    uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
    uint16_t attribute = attribute_byte << 8;

    if (c == 0x08 && cursor_x) { 		// 0x08 是退格键的 ASCII 码
        cursor_x--;
    } else if (c == 0x09) {				// 0x09 是tab键的 ASCII 码
        cursor_x = (cursor_x+8) & ~(8-1);
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c >= ' ') { 				// 记录至 video_memory
    	// len(char) = 1 byte
        video_memory[cursor_y*80 + cursor_x] = c | attribute;
        cursor_x++;
    }
    
    if (cursor_x >= 80) {				// 每 80 个字符一行，满80就必须换行了
        cursor_x = 0;
        cursor_y ++;
    }
    
    scroll(); 							// 如果需要的话滚动屏幕显示
    move_cursor(); 						// 移动硬件的输入光标
}

// 屏幕打印一个以 '\0' 结尾的字符串(默认黑底白字)
void console_write(char *cstr) {
	while (*cstr) {
	    console_putc_color(*cstr++, rc_black, rc_white);
	}
}

// 屏幕打印一个以 '\0' 结尾的字符串(带颜色)
void console_write_color(char *cstr, real_color_t back, real_color_t fore) {
	while (*cstr) {
	    console_putc_color(*cstr++, back, fore);
	}
}

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n, real_color_t back, real_color_t fore) {
	int tmp;
	int noZeroes = 1; // flag => front 0

	console_write_color("0x", back, fore); // print '0x'

	int i;
	for (i = 28; i >= 0; i -= 4) {
		tmp = (n >> i) & 0xF; // hacky
		if (tmp == 0 && noZeroes != 0) {
		    continue;
		}
		noZeroes = 0;
		if (tmp >= 0xA) {
		      console_putc_color(tmp-0xA+'a', back, fore);
		} else {
		      console_putc_color(tmp+'0', back, fore);
		}
	}
	console_putc_color('\n', rc_black, rc_white);
}

// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n, real_color_t back, real_color_t fore) {
	if (n == 0) {
		console_write_color("0", back, fore);
		return ;
	}

	char res1[32];
	char res2[32];

	int tmp = n, i = 0, j = 0;
	while (tmp > 0) {
		res1[i++] = '0'+(tmp%10);
		tmp/=10;
	} i--;

	while (i >= 0) {
		res2[j++] = res1[i--]; 
	}
	console_write_color(res2, back, fore);
	console_putc_color('\n', rc_black, rc_white);
}

/* Functions End */