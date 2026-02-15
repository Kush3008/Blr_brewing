#ifndef BLQ_COLLECTIBLE_H
#define BLQ_COLLECTIBLE_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "gameplay/game_types.h"

namespace blq
{
    struct Collectible
    {
        ItemType type;
        bn::sprite_ptr sprite;
        bn::optional<bn::sprite_animate_action<2>> animation;
        bool collected = false;
    };
}

#endif
