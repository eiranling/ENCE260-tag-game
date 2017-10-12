#include <stdlib.h>
#include "system.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "task.h"
#include "pacer.h"
#include "led.h"
#include "player.h"
#include "special.h"
#include "../fonts/font5x7_1.h"

#define NUM_PLAYERS 2 //max players
#define NUM_SPECIALS 2 //max specials
#define NUM_IR_CODES 7 //total number of IR codes
#define TIME_LIMIT 60000 // 1 min in miliseconds
#define STANDARD_SPEED 200// set standard speed
#define UP_SPEED  100// set power up speed
#define DOWN_SPEED  300//set power down speed

/* Define polling rates in Hz  */
#define NAVSWITCH_TASK_RATE 144 //Poll the NAVSWITCH at 1000 Hz

#define DISPLAY_TASK_RATE 144 // Update the display at 144Hz to reduce flickering.

char input[NUM_IR_CODES] = {'N', 'S', 'E', 'W', 'X', 'U', 'D'};

/* Polls the navswitch and sets the direction for the player
 * to move in
 * @param the current direction to be updated
*/
void get_move (Direction* current)
{
    navswitch_update();
    
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) 
    { 
        *current = NORTH;
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH))  
    {
        *current = SOUTH;
    } else if (navswitch_push_event_p (NAVSWITCH_EAST)) 
    { 
        *current = EAST;
    } else if (navswitch_push_event_p (NAVSWITCH_WEST))
    {
        *current = WEST;
    }
}

void update_game(char* received, player_t* players, Direction* move, uint8_t* other_player)
{
    if (*received == 'N') {
        *move = NORTH;
    } else if (*received == 'S') {
        *move = SOUTH;
    } else if (*received == 'E') {
        *move = EAST;
    } else if (*received == 'W') {
        *move = WEST;
    }
    move_player(players, move, other_player);
            
}

char receive_IR (char* input, player_t* players, Direction* move, uint8_t* other_player) 
{
    char received;
    uint8_t i;
    if (ir_uart_read_ready_p()) {
        received = ir_uart_getc();
        return received;

    } else {
        return '0';
    }
}

void transmit_IR (Direction* dir) {
    char direction;
    if (ir_uart_write_ready_p()) {

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
}

void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}

int main (void)
{
    
    // create variables for game
    player_t players[NUM_PLAYERS];
    
    special_t specials[NUM_SPECIALS];

    bool host = 0;
    bool slave = 0;
    
    // initialize things
    system_init ();
    ir_uart_init(); 

    tinygl_init (DISPLAY_TASK_RATE);
    tinygl_font_set (&font5x7_1);
    navswitch_init();
    
    pacer_init(1000);
    
    char character = '1';
    while (!slave && !host) {
        tinygl_update ();
        navswitch_update ();
        
        if (navswitch_push_event_p (NAVSWITCH_NORTH))
            character = '1';

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
            character = '2';

        /* TODO: Transmit the character over IR on a NAVSWITCH_PUSH
           event.  */
        if (navswitch_push_event_p (NAVSWITCH_PUSH) && ir_uart_write_ready_p()) {
            ir_uart_putc(character);
            if (character == '1') { // indicates this FK is a host, thus set this one to a host
                host = 1; 
            }
            if (character == '2') {
                slave = 1;
            }
        }
        if (!slave || !host) {
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == '1') { // indicates the other fun kit is a host, thus set this one to a slave
                    slave = 1; 
                }
                if (character == '2') {
                    host = 1;
                }
            }
        }
        
        display_character(character);
    }
    tinygl_clear();
    if (host) {
        player = 0;
        other_player = 1;
    } else {
        player = 1;
        other_player = 0;
    }
    create_players (players, player);
    create_specials (specials);
    
    led_init();
    led_set(LED1, 0);
    

    uint16_t counter = 0;
    uint16_t p2_counter = 0;
	uint8_t collected = 100;
	uint8_t catch_timeout = 0;
    uint16_t s_counter = 0;
    uint16_t s_timeout = 0;
    bool s1_state = 1;
    bool s2_state = 1;
    char move_inst;
    
    while (1)
    {
        pacer_wait();
        tinygl_draw_point(players[player].pos, 1);
        tinygl_draw_point(players[!player].pos, 1);
        tinygl_update();
        
        get_move(&players[player].current_direction);
        
    
        move_inst = receive_IR(input, players, &players[other_player].current_direction, &other_player);
        if (move_inst == 'N') {
            players[other_player].current_direction = NORTH;
        } else if (move_inst == 'E') {
            players[other_player].current_direction = EAST;
        } else if (move_inst == 'W') {
            players[other_player].current_direction = WEST;
        } else if (move_inst == 'S') {
            players[other_player].current_direction = SOUTH;
        }
        

        //    tinygl_draw_point (players[PLAYER].pos, 0); // temp turn off point to stop ghosting
        if (counter == players[player].speed) {
            counter = 0;
            transmit_IR(&players[player].current_direction);
            tinygl_draw_point(players[player].pos, 0);
            move_player(players, &players[player].current_direction, &player);
            tinygl_draw_point(players[player].pos, 1);
            tinygl_update();
        }
        
        if (p2_counter == players[other_player].speed) {
            p2_counter = 0;
            tinygl_draw_point(players[other_player].pos, 0);
            move_player(players, &players[other_player].current_direction, &other_player);
            tinygl_draw_point(players[other_player].pos, 1);
            tinygl_update();
        }
        
        
        if (s_counter % 500 == 0 && specials[1].is_active == 1) {
            s_counter = 0;
            tinygl_draw_point(specials[1].pos, !s1_state);
            s1_state = !s1_state;
        }
        
        if (s_counter % 250 == 0 && specials[0].is_active == 1) {
            tinygl_draw_point(specials[0].pos, !s2_state);
            s2_state = !s2_state;
        }
        
        if (s_timeout == 20000) {
            s_timeout = 0;
            shuffle_specials(specials);
        }
        
        if (player_caught(players) && catch_timeout > 10) {
			catch_timeout = 0;
            swap(players);
        }
		
		collected = collision_special(players, specials);
		if (collected != 100) {
			apply_special(players, specials, collected);
			collected = 100;
		}
        
		if (catch_timeout < 255) {
			catch_timeout++;
		}
        
        tinygl_draw_point(players[other_player].pos, 0);
        tinygl_draw_point(players[other_player].pos, 1);
        
        counter++;
        p2_counter++;
        s_counter++; 
        s_timeout++;
    }
}

