#ifndef TRANS_H
#define TRANS_H

#include "system.h"
#include "player.h"
#include "ir_uart.h"

/* Reads the IR buffer and assigns the value in the buffer to a char pointer.
 * 
 * @param the char pointer to write the buffered char to.
 */
void receive_IR (char* recv);

/* Transmits a char based on the Direction enum
 * @param A pointer to the direction to transmit.
 */
void transmit_IR_dir (Direction* dir);

/* 
Transmits and 'A' to signal that the game has ended.
*/
void transmit_start(void);

