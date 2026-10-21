#pragma once

#include <allegro5/allegro5.h>

namespace LSWE {
namespace Events {

    class Keyboard {
    public:
        Keyboard();

        bool get_down(int keycode) const;

        static const char* keycode_to_str(int keycode);
        static bool get_can_set_leds();
        static bool set_leds(int leds);

        const ALLEGRO_KEYBOARD_STATE& get_state() const;
    private:
        ALLEGRO_KEYBOARD_STATE m_state;
    };

} // namespace Events
} // namespace LSWE