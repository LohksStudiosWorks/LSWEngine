#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

namespace LSWE {
namespace Utility {

    class MakeTemporaryTarget {
    public:
        MakeTemporaryTarget(const MakeTemporaryTarget&) = delete;
        void operator=(const MakeTemporaryTarget&) = delete;
        MakeTemporaryTarget(MakeTemporaryTarget&&) = delete;
        void operator=(MakeTemporaryTarget&&) = delete;

        MakeTemporaryTarget(ALLEGRO_BITMAP* new_target);
        ~MakeTemporaryTarget();
    private:
        static thread_local ALLEGRO_BITMAP *m_prev_target;
        bool m_restore_target;
    };

} // namespace Utility
} // namespace LSWE