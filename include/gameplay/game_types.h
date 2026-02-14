#ifndef BLQ_GAME_TYPES_H
#define BLQ_GAME_TYPES_H

namespace blq
{
    enum class SceneType
    {
        TITLE,
        SELECT,
        GAME,
        ENDING,
        CREDITS
    };

    enum class CharacterType
    {
        KUSH,
        MANNA
    };

    enum class ItemType
    {
        MOMOS,
        TEA,
        MANCHOW,
        ROSES,
        SUNFLOWERS
    };
}

#endif
