#include <LSWE/events/mouse.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Events {

    Mouse::Mouse() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitMouse>::instance().setup();

        al_get_mouse_state(&m_state);
    }

    unsigned Mouse::get_num_axes() {
        return al_get_mouse_num_axes();
    }

    unsigned Mouse::get_num_buttons() {
        return al_get_mouse_num_buttons();
    }

    bool Mouse::set_xy(ALLEGRO_DISPLAY* display, int x, int y) {
        return al_set_mouse_xy(display, x, y);
    }

    bool Mouse::set_z(int z) {
        return al_set_mouse_z(z);
    }

    bool Mouse::set_w(int w) {
        return al_set_mouse_w(w);
    }

    bool Mouse::set_axis(int which, int value) {
        if (which < 2) return false; // use xy instead
        return al_set_mouse_axis(which, value);
    }

    void Mouse::set_precision(int precision) {
        al_set_mouse_wheel_precision(precision);
    }

    int Mouse::get_precision() {
        return al_get_mouse_wheel_precision();
    }

    int Mouse::get_axis(int axis) const {
        return al_get_mouse_state_axis(&m_state, axis);
    }

    bool Mouse::get_down(int button) const {
        return al_mouse_button_down(&m_state, button);
    }

    const ALLEGRO_MOUSE_STATE& Mouse::get_event() const {
        return m_state;
    }
    
} // namespace Events
} // namespace LSWE