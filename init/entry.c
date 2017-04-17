/* Entry.c */
#include "types.h"
#include "console.h"

int kern_entry() {
    console_clear();
    console_write_color("HELLO MY FRIEND! MY NAME IS HURLEX97.\n", rc_black, rc_red);
    console_write_color("MADE BY WASDNS!\n", rc_black, rc_red);
    return 0;
}