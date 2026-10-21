#include <LSWE/events/timeout.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Events {

    Timeout::Timeout(double seconds) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();
        
        al_init_timeout(this, seconds);
    }

} // namespace Events
} // namespace LSWE