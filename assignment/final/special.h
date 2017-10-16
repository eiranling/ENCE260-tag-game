#ifndef SPECIAL_H
#define SPECIAL_H

#include <stdlib.h>
#include "tinygl.h"
#include "player.h" 

typedef enum{SPEED_UP, SLOW_DOWN} Special;
    
typedef struct special_struct
{
    tinygl_point_t pos;
    Special special;
    bool is_active;
} special_t;


/* Turns on the LED relating to the pos of the special
 * @param the special that needs turning on.
 */
void turnon_specials (special_t* special);

/* Turns off the LED relating to the pos of the special
 * @param the special that needs turning off.
 */
void turnoff_specials (special_t* special);

/* turns off the specials leds, shuffles their positions 
 * then turns on the specials leds.
 * @param specials list to be shuffled.
 */
void shuffle_specials (special_t* specials);

/* Checks the position of the player along with the position of the specials
 * if the specials are active, and a collison has occured, 
 * turn off the led of the collided with special and return the index of that special.
 * @param the list of current players
 * @param the list of specials
 * @param the index of the player whos position is to be checked with special
 * @return the index of the special that has been collided with or -1
 */
uint8_t collision_special (player_t* players, special_t* specials, uint8_t player);

/* Applies the speedup/slowdown special to the players speed
 * @param player: the player who's speed is to be altered
 * @param specials: the list of specials
 * @param collision: the index of the collided with special
 */
void apply_special (player_t* player, special_t* specials, uint8_t collision);

/* Creates both specials and places them in random locations
 * on the matrix, ensuring they do not start in the same spot
 * set to off (!active) initially.
 * @params the list of specials to be populated
 */
void create_specials (special_t* specials);

#endif

