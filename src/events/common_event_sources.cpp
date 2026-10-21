#include <LSWE/events/common_event_sources.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Events {

    KeyboardEventSource::KeyboardEventSource() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitKeyboard>::instance().setup();
    }

    KeyboardEventSource::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_keyboard_event_source();
    }

    MouseEventSource::MouseEventSource() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitMouse>::instance().setup();
    }

    MouseEventSource::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_mouse_event_source();
    }

    TouchEventSource::TouchEventSource() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitTouch>::instance().setup();
    }

    TouchEventSource::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_touch_input_event_source();
    }

    JoystickEventSource::JoystickEventSource() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitJoystick>::instance().setup();
    }

    JoystickEventSource::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_get_joystick_event_source();
    }

} // namespace Events
} // namespace LSWE