/* Entry.c */
#include "types.h"
#include "console.h"
#include "string.h"

int kern_entry() {
    console_clear();

    printk("\n*************************************\n");
    console_write_color("HELLO MY FRIEND! MY NAME IS HURLEX97.\n", rc_black, rc_light_red);
    console_write_color("MADE BY WASDNS!\n", rc_black, rc_light_red);

    printk("\n*************************************\n");
    uint32_t num = 100;
    printk("\nTests:\n");
    console_write_dec(num, rc_black, rc_light_blue);
    console_write_hex(num, rc_black, rc_light_blue);

    printk("\nThis is a function named printk, its value is %d\n", 1123);

    printk("\n*************************************\n");
    panic("test");
    printk("\n*************************************\n");

    return 0;
}
