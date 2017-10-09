#include <stdlib.h>
#include "system.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "task.h"
#include "pacer.h"
#include "led.h"

#define NUM_PLAYERS 2 //max players
#define NUM_SPECIALS 2 //max specials
#define NUM_IR_CODES 6 //total number of IR codes
#define TIME_LIMIT 60000 // 1 min in miliseconds
#define PLAYER 0 // set player for this unit
#define STANDARD_SPEED 2// set standard speed
#define UP_SPEED  3// set power up speed
#define DOWN_SPEED  1//set power down speed

/* Define polling rates in Hz  */
#define NAVSWITCH_TASK_RATE 144 //Poll the NAVSWITCH at 1000 Hz

#define DISPLAY_TASK_RATE 144 // Update the display at 144Hz to reduce flickering.

char input[NUM_IR_CODES] = {N, S, E, W, X, U, D}

typedef enum {NORTH, EAST, WEST, SOUTH} Direction;

typedef enum{SPEED_UP, SLOW_DOWN} Special;
 

typedef struct special_struct
{
    tinygl_point_t pos;
    Special special;
    bool is_active;
} special_t;    


typedef struct player_struct
{
    tinygl_point_t pos;
    bool is_runner;
    uint8_t speed;
    Direction current_direction;
} player_t;


/* Checks to see if the players have ended up at the same co-ords
 * this will indicate if the runner has been caught by the chaser
 * @param the list of players
 * @return true if players are at same co-ords
*/
bool player_caught (player_t* players) 
{
    bool caught = false;
    if ((players[0].pos.x == players[1].pos.x) && (players[0].pos.y == players[1].pos.y))
    { 
        caught = true;
    }
    return caught;
}


/* Creates both players and places them in random locations
 * on the matrix, ensuring they do not start in the same spot
 * @params the list of players to be populated
*/
void create_players (player_t* players) 
{
    uint8_t i;
    uint8_t runner = 1;

    for (i = 0; i < NUM_PLAYERS; i++)
    {
        uint8_t x;
        uint8_t y;
        
        do { //randomly draw co-ords within our matrix
            x = rand () % TINYGL_WIDTH;
            y = rand () % TINYGL_HEIGHT;
        } while (player_caught(players)); //make sure both players start in different spaces.
    
        players[i].pos.x = x;
        players[i].pos.y = y;
        players[i].is_runner = runner;
        players[i].speed = STANDARD_SPEED;
        
        runner = !runner; //make both players have different roles
        
        tinygl_draw_point (players[i].pos, 1); // 1 for on
        if (i == PLAYER && players[i].is_runner == 0) {
            //**TODO/
            led_set(LED1, 1);
        }
    }
}



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

/* moves the player of this machiene in the direction stated 
 * @param the direction to move in
 * @param the list of players
*/
void move_player (player_t* pos, Direction* new)
{
    if (*new == SOUTH) {
        pos->y++;
        if (pos->y > TINYGL_HEIGHT) 
        {
            pos->y = 0;
        }
    } else if (*new == EAST) {
        pos->x++;
        if (pos->x > TINYGL_WIDTH) 
        {
            pos->x = 0;
        }
    } else if (*new == NORTH) {
        pos->y--;
        if (pos->y < 0) 
        {
            pos->y = TINYGL_HEIGHT;
        }
    } else if (*new == WEST) {
        pos->x--;
        if (pos->x < 0)
        {
            pos->x = TINYGL_WIDTH;
        }
    }
}


/* Swaps over the status of runner so the runner becomes the chaser
 * and the chaser becomes the runner
 * @param the list of active players
*/
void swap (player_t* players) 
{
    if (players[0].is_runner) {
        players[0].is_runner = 0;
        players[1].is_runner = 1;
        //**TODO**//
            
    } else {
        players[0].is_runner = 1;
        players[1].is_runner = 0;
        //**TODO**//
            led_set(LED1, 0);
    }
}


/* Creates both specials and places them in random locations
 * on the matrix, ensuring they do not start in the same spot
 * set to off (!active) initially.
 * @params the list of specials to be populated
 */
void create_specials (special_t* specials)
{
    uint8_t i;

    for (i = 0; i < NUM_SPECIALS; i++)
    {
        uint8_t x;
        uint8_t y;
        
        do { //randomly draw co-ords within our matrix
            x = rand () % TINYGL_WIDTH;
            y = rand () % TINYGL_HEIGHT;
        } while (i > 0 && ((x == specials[0].pos.x) && y == specials[0].pos.y )); //make sure both specials appear in different spaces.
        
        specials[i].pos.x = x;
        specials[i].pos.y = y;
        specials[i].special = i; //make sure specials have differenet Special enum values
        specials[i].is_active = 0;
         
    }
} 


/* Turns on the LED relating to the pos of the special
 * sets the flash rate of the special according to its Special enum value
 * @param the special that needs turning on.

void turnon_specials (special_t* special) 
{
    break;
      //**TODO//
    // set the led of the specials to on at rate relating to type of special.

}


/* Turns off the LED relating to the pos of the special
 * @param the special that needs turning off.

void turnoff_specials (special_t* special) 
{
    break;
      //**TODO//
    // set the led of the specials to off.
}
 
/* turns off the specials leds, shuffles their positions 
 * then turns on the specials leds.
 * @param specials list to be shuffled.
 
void shuffle_specials (special_t* specials) 
{
    uint8_t i;
    for (i = 0; i < NUM_SPECIALS; i++) 
    {
        turnoff_specials(&specials[i]);
    }
    create_specials(specials); 
    for (i = 0; i < NUM_SPECIALS; i++) 
    {   
        turnon_specials(&specials[i]);
    }
}  



/* Applies the speedup/slowdown special to the players speed
 * @param players: the list of players
 * @param specials: the list of specials
 * @param collision: the index of the collided with special
 
void apply_special (player_t* players, special_t* specials, uint8_t collision)
{

    if (specials[collision].special == SPEED_UP) {
        players[PLAYER].speed = UP_SPEED;
    } else {
        players[PLAYER].speed = DOWN_SPEED;
    }

}

/* Checks the position of the player along with the position of the specials
 * if the specials are active, and a collison has occured, 
 * turn off the led of the collided with special and return the index of that special.
 * @param the list of current players
 * @param the list of specials
 * @return the index of the special that has been collided with or -1
 
uint8_t collision_special (player_t* players, special_t* specials)
{
    uint8_t i;
    
    for (i = 0; i < NUM_SPECIALS; i++) {
        if (((players[PLAYER].pos.x == specials[i].pos.x) && (players[PLAYER].pos.y == specials[i].pos.y)) && specials[i].is_active) {
            specials[i].is_active = 0;
            turnoff_specials(&specials[i]);
            return i;
        }
    }
    return -1;
}  
*/


void receive_IR (char* input) 
{
    char recieved;
    uint8_t i;
    if (ir_uart_read_ready_p()) {
    received = ir_uart_getc();
    for (i = 0; i < NUM_IR_CODES; i++) {
        if (received == *input[i]){
            update_game(input[i]);
        }
    }
}

void update_game (char* received)
{
    switch (*received) {
        case(N):
            
}



int main (void)
{
    
    // create variables for game
    player_t players[NUM_PLAYERS];
    
    special_t specials[NUM_SPECIALS];
    
    /*int8_t collision;*/
    //Direction current_direction;
    
    
    // initialize things
    system_init ();
    ir_uart_init(); 

    tinygl_init (DISPLAY_TASK_RATE);
    navswitch_init();
    
    pacer_init(1000);
    create_players (players);
    
    create_specials (specials);
    
    led_init();

    uint8_t counter = 0;
    uint8_t s_counter = 0;
    bool s_state = 1;
    while (1)
    {
        pacer_wait();
        tinygl_draw_point(players[0].pos, 1);
        tinygl_draw_point(players[1].pos, 1);
        tinygl_update();
          //**TODO**//
          //Sets up scheduled tasks
        //task_t tasks[] = {
        //  {.func = get_move(&current_direction), .period = TASK_RATE / NAVSWITCH_TASK_RATE, .data = },
        //  {.func = update_player_pos(players, &current_direction), .period / DISPLAY_TASK_RATE}
        //}
        // set up a task scheduler to poll navswitch, 
        // place specials, IR polling,
        // update location of runner, update location of chaser.
        // (rate of runner/chaser update will depend on active specials)
        // turn off special effects after 8 seconds. (i.e .speed = STANDARD_SPEED;)
        
        get_move(&players[0].current_direction);
        
        
            
            //**TODO**//
            //Move this into it's own separate function for task scheduling
        //    tinygl_draw_point (players[PLAYER].pos, 0); // temp turn off point to stop ghosting
        if (counter == 200) {
            counter = 0;
            tinygl_draw_point(players[0].pos, 0);
            move_player(&players[0], &players[0].current_direction);
            tinygl_draw_point(players[0].pos, 1);
            tinygl_update();
        }
        
        
        if (s_counter == 175) {
            s_counter = 0;
            tinygl_draw_point(specials[1].pos, !s_state);
            s_state = !s_state;
        }
        if (player_caught(players)) {
            swap(players);
        }
        
        counter++;
        s_counter++; 
        //tinygl_draw_point (players[PLAYER].pos, 1);
            
            //**TODO**//
            //Move this into it's own separate function for task scheduling
            //if (player_caught (players)) {
             //   swap(players);
            //}
            //collision = collision_special (players, specials);
            //if (collision != -1) {
             //   apply_special(players, specials, collision);
             //   collision = -1;
            //}
    }
}
