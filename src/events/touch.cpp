#include <LSWE/events/touch.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Events {

    Touch::Touch() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        Utility::SingletonOf<Utility::AllegroInitTouch>::instance().setup();

        al_get_touch_input_state(&m_state);
    }
    
    const ALLEGRO_TOUCH_INPUT_STATE& Touch::get_event() const {
        return m_state;
    }
    
} // namespace Events
} // namespace LSWE