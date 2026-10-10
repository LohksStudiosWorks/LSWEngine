#include <LSWE/utility/make_temporary_target.hpp>

namespace LSWE {
namespace Utility {

    thread_local ALLEGRO_BITMAP *MakeTemporaryTarget::m_prev_target = nullptr;

    MakeTemporaryTarget::MakeTemporaryTarget(ALLEGRO_BITMAP* new_target)
    {
        m_prev_target = al_get_target_bitmap();

        if (m_restore_target = (m_prev_target != new_target)) 
            al_set_target_bitmap(new_target);
    }

    MakeTemporaryTarget::~MakeTemporaryTarget() {
        if (m_prev_target && m_restore_target) 
            al_set_target_bitmap(m_prev_target);
    }

} // namespace Utility
} // namespace LSWE