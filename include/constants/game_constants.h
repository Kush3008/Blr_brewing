#ifndef BLQ_GAME_CONSTANTS_H
#define BLQ_GAME_CONSTANTS_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"

namespace blq::constants
{
    constexpr int WORLD_WIDTH = 256;
    constexpr int WORLD_HEIGHT = 256;

    constexpr bn::fixed PLAYER_SPEED = 1;
    constexpr int REQUIRED_ITEM_COUNT = 1;

    constexpr bn::fixed_point MEETING_SPOT(104, 96);
}

#endif
