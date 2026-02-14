#include "bn_array.h"
#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_optional.h"
#include "bn_regular_bg_items_bangalore_street.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_variable_8x16_font.h"
#include "bn_sprite_items_gift_icons.h"
#include "bn_sprite_items_heart_blue.h"
#include "bn_sprite_items_heart_purple.h"
#include "bn_sprite_items_heart_red.h"
#include "bn_sprite_items_icon_manchow.h"
#include "bn_sprite_items_icon_momos.h"
#include "bn_sprite_items_icon_roses.h"
#include "bn_sprite_items_icon_sunflowers.h"
#include "bn_sprite_items_icon_tea.h"
#include "bn_sprite_items_kush.h"
#include "bn_sprite_items_manchow.h"
#include "bn_sprite_items_manna.h"
#include "bn_sprite_items_meeting_marker.h"
#include "bn_sprite_items_momos.h"
#include "bn_sprite_items_roses.h"
#include "bn_sprite_items_sunflowers.h"
#include "bn_sprite_items_tea.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_to_string.h"
#include "bn_vector.h"

#include "constants/game_constants.h"
#include "gameplay/game_types.h"

namespace blq
{

struct SaveData
{
    CharacterType chosen_character = CharacterType::KUSH;
};

class TitleScene
{
public:
    explicit TitleScene(bn::sprite_text_generator& text_generator)
    {
        _bg.emplace(bn::regular_bg_items::bangalore_street.create_bg(0, 0));
        text_generator.generate(0, -42, "Bangalore Love Quest", _text);
        text_generator.generate(0, 20, "Press START", _text);
    }

    bool update()
    {
        if(bn::keypad::start_pressed())
        {
            bn::sound_items::confirm.play();
            return true;
        }
        return false;
    }

private:
    bn::optional<bn::regular_bg_ptr> _bg;
    bn::vector<bn::sprite_ptr, 64> _text;
};

class SelectScene
{
public:
    SelectScene(bn::sprite_text_generator& text_generator, SaveData& save_data) : _save_data(save_data)
    {
        _bg.emplace(bn::regular_bg_items::bangalore_street.create_bg(0, 0));
        _kush.emplace(bn::sprite_items::kush.create_sprite(-38, -4));
        _manna.emplace(bn::sprite_items::manna.create_sprite(38, -4));

        text_generator.generate(0, -56, "Choose Character", _text);
        text_generator.generate(0, 54, "LEFT/RIGHT + START", _text);
        _refresh_cursor();
    }

    bool update()
    {
        if(bn::keypad::left_pressed() || bn::keypad::right_pressed())
        {
            _kush_selected = ! _kush_selected;
            _refresh_cursor();
            bn::sound_items::cancel.play();
        }

        if(bn::keypad::start_pressed() || bn::keypad::a_pressed())
        {
            _save_data.chosen_character = _kush_selected ? CharacterType::KUSH : CharacterType::MANNA;
            bn::sound_items::confirm.play();
            return true;
        }

        return false;
    }

private:
    void _refresh_cursor()
    {
        _kush->set_scale(_kush_selected ? 1.4 : 1.0);
        _manna->set_scale(_kush_selected ? 1.0 : 1.4);
    }

    SaveData& _save_data;
    bn::optional<bn::regular_bg_ptr> _bg;
    bn::optional<bn::sprite_ptr> _kush;
    bn::optional<bn::sprite_ptr> _manna;
    bn::vector<bn::sprite_ptr, 64> _text;
    bool _kush_selected = true;
};

class GameScene
{
public:
    GameScene(bn::sprite_text_generator& text_generator, SaveData& save_data) : _text_generator(text_generator)
    {
        _bg.emplace(bn::regular_bg_items::bangalore_street.create_bg(0, 0));
        if(save_data.chosen_character == CharacterType::KUSH)
        {
            _player.emplace(bn::sprite_items::kush.create_sprite(0, 0));
        }
        else
        {
            _player.emplace(bn::sprite_items::manna.create_sprite(0, 0));
        }

        _spawn_collectibles();
        _spawn_ui_icons();
        _meeting_marker.emplace(bn::sprite_items::meeting_marker.create_sprite(constants::MEETING_SPOT.x(), constants::MEETING_SPOT.y()));
        _meeting_marker->set_visible(false);
        _refresh_ui_text();
    }

    bool update()
    {
        _move_player();
        _animate_walk();
        _animate_item_bob();
        _collect_items();
        _refresh_ui_text();

        if(_all_collected)
        {
            _meeting_marker->set_visible(true);
            const bn::fixed dx = _player->x() - constants::MEETING_SPOT.x();
            const bn::fixed dy = _player->y() - constants::MEETING_SPOT.y();
            if(bn::abs(dx) < 10 && bn::abs(dy) < 10)
            {
                bn::sound_items::confirm.play();
                return true;
            }
        }

        return false;
    }

private:
    void _spawn_collectibles()
    {
        _collectibles[0].sprite.emplace(bn::sprite_items::momos.create_sprite(-88, -54));
        _collectibles[1].sprite.emplace(bn::sprite_items::tea.create_sprite(72, -42));
        _collectibles[2].sprite.emplace(bn::sprite_items::manchow.create_sprite(60, 54));
        _collectibles[3].sprite.emplace(bn::sprite_items::roses.create_sprite(-70, 62));
        _collectibles[4].sprite.emplace(bn::sprite_items::sunflowers.create_sprite(-10, -8));

        for(int i = 0; i < 5; ++i)
        {
            _collectibles[i].base_y = _collectibles[i].sprite->y();
            _collectibles[i].sprite->set_z_order(1);
        }
    }

    void _spawn_ui_icons()
    {
        _ui_icons[0].emplace(bn::sprite_items::icon_momos.create_sprite(-110, -72));
        _ui_icons[1].emplace(bn::sprite_items::icon_tea.create_sprite(-86, -72));
        _ui_icons[2].emplace(bn::sprite_items::icon_manchow.create_sprite(-62, -72));
        _ui_icons[3].emplace(bn::sprite_items::icon_roses.create_sprite(-38, -72));
        _ui_icons[4].emplace(bn::sprite_items::icon_sunflowers.create_sprite(-14, -72));

        for(auto& icon : _ui_icons)
        {
            icon->set_bg_priority(0);
            icon->set_z_order(2);
        }
    }

    void _move_player()
    {
        bn::fixed x = _player->x();
        bn::fixed y = _player->y();

        if(bn::keypad::left_held())
        {
            x -= constants::PLAYER_SPEED;
        }
        if(bn::keypad::right_held())
        {
            x += constants::PLAYER_SPEED;
        }
        if(bn::keypad::up_held())
        {
            y -= constants::PLAYER_SPEED;
        }
        if(bn::keypad::down_held())
        {
            y += constants::PLAYER_SPEED;
        }

        x = bn::max(bn::fixed(-110), bn::min(bn::fixed(110), x));
        y = bn::max(bn::fixed(-70), bn::min(bn::fixed(70), y));
        _player->set_position(x, y);
    }

    void _animate_walk()
    {
        ++_walk_frame;
        const int tile_index = (_walk_frame / 12) % 2;
        _player->set_tiles(_player->tiles_item().create_tiles(tile_index));
    }

    void _animate_item_bob()
    {
        ++_bob_counter;
        const bn::fixed bob = (_bob_counter / 20) % 2 == 0 ? 0 : -1;

        for(int i = 0; i < 5; ++i)
        {
            if(! _collectibles[i].collected)
            {
                _collectibles[i].sprite->set_y(_collectibles[i].base_y + bob);
            }
        }
    }

    void _collect_items()
    {
        int done = 0;

        for(auto& item : _collectibles)
        {
            if(item.collected)
            {
                ++done;
                continue;
            }

            const bn::fixed dx = _player->x() - item.sprite->x();
            const bn::fixed dy = _player->y() - item.sprite->y();
            if(bn::abs(dx) < 12 && bn::abs(dy) < 12)
            {
                item.collected = true;
                item.sprite->set_visible(false);
                bn::sound_items::collect.play();
                ++done;
            }
        }

        _all_collected = done == 5;
    }

    void _refresh_ui_text()
    {
        _ui_text.clear();

        int done = 0;
        for(const auto& item : _collectibles)
        {
            done += item.collected ? 1 : 0;
        }

        bn::string<24> label("Items ");
        label += bn::to_string<2>(done);
        label += "/5";
        _text_generator.generate(44, -72, label, _ui_text);

        if(_all_collected)
        {
            _text_generator.generate(36, 64, "Go to marker", _ui_text);
        }
    }

    struct Collectible
    {
        bn::optional<bn::sprite_ptr> sprite;
        bn::fixed base_y = 0;
        bool collected = false;
    };

    bn::sprite_text_generator& _text_generator;
    bn::optional<bn::regular_bg_ptr> _bg;
    bn::optional<bn::sprite_ptr> _player;
    bn::array<Collectible, 5> _collectibles;
    bn::array<bn::optional<bn::sprite_ptr>, 5> _ui_icons;
    bn::optional<bn::sprite_ptr> _meeting_marker;
    bn::vector<bn::sprite_ptr, 48> _ui_text;
    int _walk_frame = 0;
    int _bob_counter = 0;
    bool _all_collected = false;
};

class EndingScene
{
public:
    EndingScene(bn::sprite_text_generator& text_generator, const SaveData& save_data)
        : _text_generator(text_generator)
    {
        _bg.emplace(bn::regular_bg_items::bangalore_street.create_bg(0, 0));
        if(save_data.chosen_character == CharacterType::KUSH)
        {
            _left.emplace(bn::sprite_items::kush.create_sprite(-24, 20));
            _right.emplace(bn::sprite_items::manna.create_sprite(24, 20));
        }
        else
        {
            _left.emplace(bn::sprite_items::manna.create_sprite(-24, 20));
            _right.emplace(bn::sprite_items::kush.create_sprite(24, 20));
        }

        _text_generator.generate(0, -62, "At the meeting spot...", _text);
    }

    bool update()
    {
        ++_timer;

        if(_phase == 0 && _timer > 30)
        {
            _pop_heart(0);
        }
        else if(_phase == 1 && _timer > 72)
        {
            _pop_heart(1);
        }
        else if(_phase == 2 && _timer > 114)
        {
            _pop_heart(2);
        }
        else if(_phase == 3 && _timer == 160)
        {
            _gift.emplace(bn::sprite_items::gift_icons.create_sprite(0, -12));
            _text_generator.generate(0, 58, "A gift and flowers", _text);
            bn::sound_items::confirm.play();
        }
        else if(_timer > 260)
        {
            return true;
        }

        if(_heart)
        {
            if(_heart_scale < 1.6)
            {
                _heart_scale += 0.08;
            }
            else
            {
                _heart_scale -= 0.04;
                if(_heart_scale < 1.0)
                {
                    _heart_scale = 1.0;
                }
            }

            _heart->set_scale(_heart_scale);
        }

        return false;
    }

private:
    void _pop_heart(int index)
    {
        ++_phase;
        _heart_scale = 0.2;

        if(index == 0)
        {
            _heart.emplace(bn::sprite_items::heart_red.create_sprite(0, -18));
        }
        else if(index == 1)
        {
            _heart.emplace(bn::sprite_items::heart_blue.create_sprite(0, -18));
        }
        else
        {
            _heart.emplace(bn::sprite_items::heart_purple.create_sprite(0, -18));
        }

        bn::sound_items::heart_pop.play();
    }

    bn::sprite_text_generator& _text_generator;
    bn::optional<bn::regular_bg_ptr> _bg;
    bn::optional<bn::sprite_ptr> _left;
    bn::optional<bn::sprite_ptr> _right;
    bn::optional<bn::sprite_ptr> _heart;
    bn::optional<bn::sprite_ptr> _gift;
    bn::vector<bn::sprite_ptr, 48> _text;
    int _timer = 0;
    int _phase = 0;
    bn::fixed _heart_scale = 1.0;
};

class CreditsScene
{
public:
    explicit CreditsScene(bn::sprite_text_generator& text_generator)
    {
        _bg.emplace(bn::regular_bg_items::bangalore_street.create_bg(0, 0));
        text_generator.generate(0, -48, "Credits", _text);
        text_generator.generate(0, -22, "Design+Code: Original", _text);
        text_generator.generate(0, 0, "Built with Butano", _text);
        text_generator.generate(0, 24, "START to restart", _text);
    }

    bool update()
    {
        if(bn::keypad::start_pressed())
        {
            bn::sound_items::confirm.play();
            return true;
        }
        return false;
    }

private:
    bn::optional<bn::regular_bg_ptr> _bg;
    bn::vector<bn::sprite_ptr, 48> _text;
};

}

int main()
{
    bn::core::init();

    bn::sprite_text_generator text_generator(bn::sprite_items::variable_8x16_font);
    text_generator.set_center_alignment();

    blq::SaveData save_data;
    blq::SceneType scene_type = blq::SceneType::TITLE;

    while(true)
    {
        if(scene_type == blq::SceneType::TITLE)
        {
            blq::TitleScene scene(text_generator);
            while(! scene.update())
            {
                bn::core::update();
            }
            scene_type = blq::SceneType::SELECT;
        }
        else if(scene_type == blq::SceneType::SELECT)
        {
            blq::SelectScene scene(text_generator, save_data);
            while(! scene.update())
            {
                bn::core::update();
            }
            scene_type = blq::SceneType::GAME;
        }
        else if(scene_type == blq::SceneType::GAME)
        {
            blq::GameScene scene(text_generator, save_data);
            while(! scene.update())
            {
                bn::core::update();
            }
            scene_type = blq::SceneType::ENDING;
        }
        else if(scene_type == blq::SceneType::ENDING)
        {
            blq::EndingScene scene(text_generator, save_data);
            while(! scene.update())
            {
                bn::core::update();
            }
            scene_type = blq::SceneType::CREDITS;
        }
        else
        {
            blq::CreditsScene scene(text_generator);
            while(! scene.update())
            {
                bn::core::update();
            }
            save_data = blq::SaveData();
            scene_type = blq::SceneType::TITLE;
        }
    }
}
