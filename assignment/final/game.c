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
#define TIME_LIMIT 60 // 1 min in seconds
#define STANDARD_SPEED 200// set standard speed

/* Define polling rates in Hz  */
#define NAVSWITCH_TASK_RATE 144 //Poll the NAVSWITCH at 1000 Hz

#define DISPLAY_TASK_RATE 144 // Update the display at 144Hz to reduce flickering.

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

void receive_IR (char* recv) 
{
	if (ir_uart_read_ready_p()) {
		*recv = ir_uart_getc();
	}
}


void transmit_IR_dir (Direction* dir) {
    char direction = '0';
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

void transmit_end(void) {
    ir_uart_putc_nocheck('X');
}

void transmit_start(void) {
    ir_uart_putc_nocheck('A');
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
    
    create_specials (specials);
    char character = '1';
    while (!slave && !host) {
        tinygl_update ();
        navswitch_update ();
        
        if (navswitch_push_event_p (NAVSWITCH_NORTH))
            character = '1';

        if (navswitch_push_event_p (NAVSWITCH_SOUTH))
            character = '2';

        if (navswitch_push_event_p (NAVSWITCH_PUSH) && ir_uart_write_ready_p()) {
            ir_uart_putc(character);
            if (character == '1') { // indicates this FK is a host, thus set this one to a host
                host = 1; 
            }
            if (character == '2') {
                slave = 1;
            }
        }
        if (!slave && !host) {
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
    led_init();
    led_set(LED1, 0);
    create_players (players, player);

    uint16_t counter = 0;
    uint16_t p2_counter = 0;
	int8_t collected = -1;
	uint16_t catch_timeout = 3000;
    uint16_t s_counter = 0;
    uint16_t s_timeout = 0; // time out for the specials TODO: remove and replace with game_time
    uint16_t game_time = 0;
    uint16_t seconds_counter = 0;
    bool caught = 0;
    bool s1_state = 1;
    bool s2_state = 1;
    char recv_char;
    
	if (ir_uart_read_ready_p()) {
		do {
			receive_IR(&recv_char);
		} while (recv_char != 'A');
	} else {
		transmit_start();
	}
	
	while (recv_char != 'A') {
		receive_IR(&recv_char);
	}
	
	transmit_start();

    
    while (game_time <= TIME_LIMIT) // game runs for a minute.
    {
        
        pacer_wait();
        tinygl_draw_point(players[player].pos, 1);
        tinygl_draw_point(players[other_player].pos, 1);
		tinygl_update();

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
        if (slave) {
            if (recv_char == 'X') {
                game_time = TIME_LIMIT;
            }
        }
            
        // updates the player position
        if (counter == players[player].speed) {
            counter = 0;
            get_move(&players[player].current_direction);
            transmit_IR_dir(&players[player].current_direction);
            tinygl_draw_point(players[player].pos, 0);
            move_player(players, &players[player].current_direction, &player);
            tinygl_draw_point(players[player].pos, 1);
            tinygl_update();
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
        if (s_counter % 500 == 0 && specials[1].is_active == 1) {
            s_counter = 0;
            tinygl_draw_point(specials[1].pos, !s1_state);
            s1_state = !s1_state;
        }
        
        // cause the second powerup to blink faster
        if (s_counter % 250 == 0 && specials[0].is_active == 1) {
            tinygl_draw_point(specials[0].pos, !s2_state);
            s2_state = !s2_state;
        }
        
        // creates a timeout for the powerups, shuffles every 15 seconds.
        if (game_time % 15) {
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
		
		if (!players[player].is_runner) {
			led_set(LED1, 0);
		} else {
			led_set(LED1, 1);
        }
        
        // detects if one player has caught the other, as well as have a cooldown to ensure that the roles don't switch too quickly.
        if (player_caught(players) && !caught) {
            caught = 1;
            swap(players);
            players[player].speed = STANDARD_SPEED;
            players[other_player].speed = STANDARD_SPEED;
        }
		
        // Only resets the caught flag when the players are apart.
		if (!player_caught(players)) {
			caught = 0;
		}
        
        // increment the counters
        if (catch_timeout < 3000) { // Stops incrementing past a point so that it won't reset to 0
            catch_timeout++;
        }
        
        // Cycle is 1000 Hz, or every 1 ms, so every thousand cycles should be 1 second in real time.
        if (seconds_counter == 1000) {
            seconds_counter = 0;
            game_time++;
        }
        
        counter++;
        p2_counter++;
        s_counter++; 
        s_timeout++;
        seconds_counter++;
    }
    
    if (host) {
        transmit_end();
    }
    
    if (players[player].is_runner) {
        tinygl_text("YOU LOSE");
    } else {
        tinygl_text("YOU WIN");
    }
    
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(10);
    pacer_init(150);
    while (1) {
        pacer_wait();
        tinygl_update();
    }
}

