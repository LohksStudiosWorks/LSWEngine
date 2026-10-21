#include <LSWE/events/joystick.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Events {

    const ALLEGRO_JOYSTICK_STATE& Joystick::get_state() const {
        return m_state;
    }

    Joystick::Joystick(ALLEGRO_JOYSTICK* from_device) {
        al_get_joystick_state(from_device, &m_state);
    }

    JoystickDevice JoystickDevice::get(int num) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitJoystick>::instance().setup();

        return JoystickDevice(al_get_joystick(num));
    }

    bool JoystickDevice::reconfigure() {
        return al_reconfigure_joysticks();
    }

    int JoystickDevice::get_num_joysticks() {
        return al_get_num_joysticks();
    }

    ALLEGRO_JOYSTICK_TYPE JoystickDevice::get_type() const {
        return al_get_joystick_type(m_joystick.get());
    }

    ALLEGRO_JOYSTICK_GUID JoystickDevice::get_guid() const {
        return al_get_joystick_guid(m_joystick.get());
    }

    Joystick JoystickDevice::get_state() const {
        return Joystick(m_joystick.get());
    }

    bool JoystickDevice::set_mappings(const char* filename) {
        return al_set_joystick_mappings(filename);
    }

    bool JoystickDevice::set_mappings(ALLEGRO_FILE* file) {
        return al_set_joystick_mappings_f(file);
    }

    JoystickDevice::JoystickDevice(ALLEGRO_JOYSTICK* JoystickDevice)
        : m_joystick(JoystickDevice, al_release_joystick)
    {
        if (!m_joystick)
            throw Exception::NullException("JoystickDevice was null");
    }
    
} // namespace Events
} // namespace LSWE