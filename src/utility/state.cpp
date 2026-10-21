#include <LSWE/utility/state.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    State State::store(int flags) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        ALLEGRO_STATE* state = new ALLEGRO_STATE();
        al_store_state(state, flags);
        return State(state);
    }

    void State::restore() {
        al_restore_state(m_state.get());
    }

    State::State(ALLEGRO_STATE* state)
        : m_state(state)
    {
        if (!m_state)
            throw Exception::NullException("State was null");
    }


} // namespace Utility
} // namespace LSWE