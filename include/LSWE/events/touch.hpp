#pragma once

#include <allegro5/allegro5.h>

namespace LSWE {
namespace Events {

    class Touch {
    public:
        Touch();

        const ALLEGRO_TOUCH_INPUT_STATE& get_event() const;
    private:
        ALLEGRO_TOUCH_INPUT_STATE m_state;
    };

} // namespace Events
} // namespace LSWE