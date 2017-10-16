#include <stdlib.h> 
#include "special.h"
#include "tinygl.h"



/* Turns on the LED relating to the pos of the special
 * @param the special that needs turning on.
 */
void turnon_specials (special_t* special) 
{   
    special->is_active = 1;
    tinygl_draw_point(special->pos, 1);
}

/* Turns off the LED relating to the pos of the special
 * @param the special that needs turning off.
 */
void turnoff_specials (special_t* special) 
{
    tinygl_draw_point(special->pos, 0);
    special->pos.x = -1;
    special->pos.y = -1;
}

/* turns off the specials leds, shuffles their positions 
 * then turns on the specials leds.
 * @param specials list to be shuffled.
 */
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

/* Checks the position of the player along with the position of the specials
 * if the specials are active, and a collison has occured, 
 * turn off the led of the collided with special and return the index of that special.
 * @param the list of current players
 * @param the list of specials
 * @param the index of the player whos position is to be checked with special
 * @return the index of the special that has been collided with or -1
 */
int8_t collision_special (player_t* players, special_t* specials, uint8_t player)
{
    uint8_t i;
    
    for (i = 0; i < NUM_SPECIALS; i++) {
        if (((players[player].pos.x == specials[i].pos.x) && (players[player].pos.y == specials[i].pos.y)) && specials[i].is_active) {
            specials[i].is_active = 0;
            turnoff_specials(&specials[i]);
            return i;
        }
    }
    return -1;
}  

/* Applies the speedup/slowdown special to the players speed
 * @param player: the player who's speed is to be altered
 * @param specials: the list of specials
 * @param collision: the index of the collided with special
 */
void apply_special (player_t* player, special_t* specials, uint8_t collision)
{

    if (specials[collision].special == SPEED_UP) {
            if (player->speed - CHANGE_SPEED >= MAX_SPEED) {
            player->speed -= CHANGE_SPEED;
        } else {
            player->speed = CHANGE_SPEED;
        }
    } else {
        if (player->speed + CHANGE_SPEED <= MIN_SPEED) {
            player->speed += CHANGE_SPEED;
        } else {
            player->speed = MIN_SPEED;
        }
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
        } while (i > 0 && ((x == specials[0].pos.x) && y == specials[0].pos.y)); //make sure both specials appear in different spaces.
        
        specials[i].pos.x = x;
        specials[i].pos.y = y;
        specials[i].special = i; //make sure specials have differenet Special enum values
        specials[i].is_active = 0;
         
    }
} 

