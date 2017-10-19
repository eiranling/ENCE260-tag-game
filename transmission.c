/**
    @file transmission.c
    @authors Eiran Ling and Susan Collishaw
    @date 18 Oct 2017
    @brief The transmission module for the tag game
*/

#include "ir_uart.h"
#include "player.h"

/* Reads a char from the IR buffer */
void receive_IR (char* recv) 
{
    if (ir_uart_read_ready_p()) {
        *recv = ir_uart_getc();
    }
}

/* transmits a char based on a given direction enum */
void transmit_IR_dir (Direction* dir) 
{
    char direction = '0';
        if (*dir == NORTH) {
            direction = 'N';
        } else if (*dir == EAST) {
            direction = 'E';
        } else if (*dir == WEST) {
            direction = 'W';
        } else if (*dir == SOUTH) {
            direction = 'S';
        }
        ir_uart_putc(direction);
}

/* Transmits the start game signal*/
void transmit_start(void) 
{
    ir_uart_putc_nocheck('A');
}
