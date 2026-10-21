#pragma once

#include <allegro5/allegro5.h>

#include <memory>

namespace LSWE {
namespace Events {

    class JoystickDevice;

    class Joystick {
    public:
        const ALLEGRO_JOYSTICK_STATE& get_state() const;
    private:
        Joystick(ALLEGRO_JOYSTICK* from_device);

        friend class JoystickDevice;

        ALLEGRO_JOYSTICK_STATE m_state;
    };

    class JoystickDevice {
    public:
        static JoystickDevice get(int num);

        static bool reconfigure();
        static int get_num_joysticks();

        bool get_active() const;
        const char* get_name() const;
        const char* get_stick_name(int stick) const;
        const char* get_axis_name(int stick, int axis) const;
        const char* get_button_name(int button) const;
        int get_stick_flags(int stick) const;

        int get_num_sticks() const;
        int get_num_axes(int stick) const;
        int get_num_buttons() const;

        ALLEGRO_JOYSTICK_TYPE get_type() const;
        ALLEGRO_JOYSTICK_GUID get_guid() const;

        Joystick get_state() const;

        bool set_mappings(const char* filename);
        bool set_mappings(ALLEGRO_FILE* file);
    private:
        JoystickDevice(ALLEGRO_JOYSTICK* JoystickDevice);

        std::shared_ptr<ALLEGRO_JOYSTICK> m_joystick;
    };

} // namespace Events
} // namespace LSWE