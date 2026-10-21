#pragma once

#include <allegro5/allegro5.h>

#include <memory>

namespace LSWE {
namespace Utility {
    
    class State {
    public:
        static State store(int flags);

        void restore();
    private:
        State(ALLEGRO_STATE* state);

        std::shared_ptr<ALLEGRO_STATE> m_state;
    };

} // namespace Utility
} // namespace LSWE