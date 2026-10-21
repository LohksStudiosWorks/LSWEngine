#pragma once

#include <allegro5/allegro5.h>

namespace LSWE {
namespace Events {

    class Mouse {
    public:
        Mouse();

        static unsigned get_num_axes();
        static unsigned get_num_buttons();
        static bool set_xy(ALLEGRO_DISPLAY* display, int x, int y);
        static bool set_z(int z);
        static bool set_w(int w);
        static bool set_axis(int which, int value);
        static void set_precision(int precision);

        static int get_precision();

        int get_axis(int axis) const;
        bool get_down(int button) const;

        const ALLEGRO_MOUSE_STATE& get_event() const;
    private:
        ALLEGRO_MOUSE_STATE m_state;
    };

} // namespace Events
} // namespace LSWE