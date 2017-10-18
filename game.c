/**
	The main module for the tag game. 
	@authors: Susan Collishaw, Eiran Ling
    @date: 18 Oct 2017
    @brief: The main game

    @defgroup game The main game

    This module handles most if not all the function calls required to
    run the game
*/

#include <stdlib.h>
#include "system.h"
#include "pio.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "pacer.h"
#include "led.h"
#include "player.h"
#include "special.h"
#include "transmission.h"
#include "../fonts/font5x7_1.h"

#define OVERFLOW 65535
#define SHUFFLE_TIME 15000

#define NUM_PLAYERS 2 //max players
#define NUM_SPECIALS 2 //max specials
#define NUM_IR_CODES 7 //total number of IR codes
#define TIME_LIMIT 60 // 1 min in seconds
#define STANDARD_SPEED 200// set standard speed

#define PIEZO1_PIO PIO_DEFINE (PORT_D, 4)
#define PIEZO2_PIO PIO_DEFINE (PORT_D, 6)

// Pace the loop at 1000Hz, or 1 ms.
#define PACER_RATE 1000
// Update the display at 1000Hz to negate flickering.
#define DISPLAY_TASK_RATE 1000
// Blink an LED at a slow rate
#define SLOW_BLINK_RATE PACER_RATE/2
// Blink an LED at a faster rate
#define FAST_BLINK_RATE PACER_RATE/4
// Defines how long the buzzer should beep for.
#define BEEP_TIME PACER_RATE/2

#define DISPLAY_TEXT_SPEED 10

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

int main (void)
{
    
    // create variables for game
    player_t players[NUM_PLAYERS];
    special_t specials[NUM_SPECIALS];

    bool host = 0;
    bool slave = 0;
    
    // initialize all the required peripherals.
    system_init ();
	pio_config_set (PIEZO1_PIO, PIO_OUTPUT_LOW);
    pio_config_set (PIEZO2_PIO, PIO_OUTPUT_LOW);
    ir_uart_init(); 

    tinygl_init (DISPLAY_TASK_RATE);
    tinygl_font_set (&font5x7_1);
	tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(DISPLAY_TEXT_SPEED);
    navswitch_init();
    
    pacer_init(PACER_RATE);
    
    create_specials (specials);
	tinygl_text("Push button to start");
	
    char character = '1';
    while (!slave && !host) {
        pacer_wait();
        tinygl_update ();
        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            ir_uart_putc(character);
            host = 1;
		} else {
            if (!slave && !host) {
                if (ir_uart_read_ready_p()) {
                    character = ir_uart_getc();
                    if (character == '1') { // indicates the other fun kit is a host, thus set this one to a slave
                        slave = 1; 
                    }
                }
            }
        }
    }
    tinygl_clear();
    
    if (host) {
        player = 0;
        other_player = 1;
    } else {
        player = 1;
        other_player = 0;
    }
    
    led_init();
    led_set(LED1, 0);

    // Create the players
    create_players (players, player);
    
    // Declare and initialize variables
    uint16_t counter = 0;
    uint16_t p2_counter = 0;
    int8_t collected = -1;
    uint16_t catch_timeout = OVERFLOW;
    uint16_t s_counter = 0;
    uint16_t s_timeout = 0;
    uint16_t game_time = 0;
    uint16_t seconds_counter = 0;
	uint16_t beep_counter = BEEP_TIME;
    bool caught = 0;
    bool s1_state = 1;
    bool s2_state = 1;
    char recv_char;
    
    while (game_time <= TIME_LIMIT) // game runs for a minute.
    {
        pacer_wait();
        tinygl_draw_point(players[player].pos, 1);
        tinygl_draw_point(players[other_player].pos, 1);
        tinygl_update();
            
        // updates the player position
        if (counter == players[player].speed) {
            counter = 0;
            get_move(&players[player].current_direction);
            if (ir_uart_read_ready_p()) {
                recv_char = ir_uart_getc();
            }
            transmit_IR_dir(&players[player].current_direction);
            tinygl_draw_point(players[player].pos, 0);
            move_player(players, &players[player].current_direction, &player);
            tinygl_draw_point(players[player].pos, 1);
            tinygl_update();
        }
        
        // Retrieve the character from the buffer and translate it into a direction.
        receive_IR(&recv_char);
        if (recv_char == 'N') {
           players[other_player].current_direction = NORTH;
        } else if (recv_char == 'E') {
           players[other_player].current_direction = EAST;
        } else if (recv_char == 'W') {
            players[other_player].current_direction = WEST;
        } else if (recv_char == 'S') {
            players[other_player].current_direction = SOUTH;
        }

        // updates the second player's position
        if (p2_counter == players[other_player].speed) {
            p2_counter = 0;
            tinygl_draw_point(players[other_player].pos, 0);
            move_player(players, &players[other_player].current_direction, &other_player);
            tinygl_draw_point(players[other_player].pos, 1);
            tinygl_update();
        }
        
        // cause the first powerup to blink slowly
        if (s_counter % SLOW_BLINK_RATE == 0 && specials[SLOW_DOWN].is_active == 1) {
            s_counter = 0;
            tinygl_draw_point(specials[SLOW_DOWN].pos, !s1_state);
            s1_state = !s1_state;
        }
        
        // cause the second powerup to blink faster
        if (s_counter % FAST_BLINK_RATE == 0 && specials[SPEED_UP].is_active == 1) {
            tinygl_draw_point(specials[SPEED_UP].pos, !s2_state);
            s2_state = !s2_state;
        }
        
        // creates a timeout for the powerups, shuffles every 15 seconds.
        if (s_timeout == SHUFFLE_TIME) {
            s_timeout = 0;
            shuffle_specials(specials);
        }
        // code to detect and apply powerup when picked up by player 1
        collected = collision_special(players, specials, player);
        if (collected != -1) {
            apply_special(&players[player], specials, collected);
        }
        
        // code to detect and apply power when picked up by player 2.
        collected = collision_special(players, specials, other_player);
        if (collected != -1) {
            apply_special(&players[other_player], specials, collected);
        }
        

        // detects if one player has caught the other, as well as have a cooldown to ensure that the roles don't switch too quickly.
        if (player_caught(players) && !caught) {
            caught = 1;
            swap(players);
            players[player].speed = STANDARD_SPEED;
            players[other_player].speed = STANDARD_SPEED;
			if (players[player].is_runner) {
                // Turn LED on if the player is chaser
				led_set(LED1, 1);
			} else {
                // Turn LED off if the player is not chaser.
				led_set(LED1, 0);
			}
			beep_counter = 0;
        }
		
        // Plays a soft beep for half a second when the players are caught.
		if (caught == 1 || beep_counter < BEEP_TIME) {
			pio_output_low(PIEZO1_PIO);
			pio_output_low(PIEZO2_PIO);
			beep_counter++;
		} else {
			pio_output_high(PIEZO1_PIO);
			pio_output_high(PIEZO2_PIO);
		}
        
        // Only resets the caught flag when the players are apart.
        if (!player_caught(players)) {
            caught = 0;
        }
        
        // increment the counters
        if (catch_timeout < OVERFLOW) { // Stops incrementing past a point so that it won't reset to 0
            catch_timeout++;
        }
        
        // Counts when a second in real time has passed.
        if (seconds_counter == PACER_RATE) {
            seconds_counter = 0;
            game_time++;
        }
        
        counter++;
        p2_counter++;
        s_counter++; 
        s_timeout++;
        seconds_counter++;
    }
    
    if (players[player].is_runner) {
        tinygl_text("YOU LOSE");
    } else {
        tinygl_text("YOU WIN");
    }
    
    while (1) {
        pacer_wait();
        tinygl_update();
    }
}

