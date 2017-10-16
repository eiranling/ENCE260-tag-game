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

void turnon_specials (special_t* special);

void turnoff_specials (special_t* special);

void shuffle_specials (special_t* specials);

int8_t collision_special (player_t* players, special_t* specials, uint8_t player);

void apply_special (player_t* player, special_t* specials, uint8_t collision);

void create_specials (special_t* specials);

#endif

