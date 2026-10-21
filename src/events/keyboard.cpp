#include <LSWE/events/keyboard.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Events {

    Keyboard::Keyboard() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitKeyboard>::instance().setup();

        al_get_keyboard_state(&m_state);
    }

    bool Keyboard::get_down(int keycode) const {
        return al_key_down(&m_state, keycode);
    }

    const char* Keyboard::keycode_to_str(int keycode) {
        return al_keycode_to_name(keycode);
    }

    bool Keyboard::get_can_set_leds() {
        return al_can_set_keyboard_leds();
    }

    bool Keyboard::set_leds(int leds) {
        al_set_keyboard_leds(leds);
    }

    const ALLEGRO_KEYBOARD_STATE& Keyboard::get_state() const {
        return m_state;
    }

} // namespace Events
} // namespace LSWE