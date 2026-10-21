#pragma once

#include <allegro5/allegro5.h>

namespace LSWE {
namespace Events {

    class Timeout : public ALLEGRO_TIMEOUT {
    public:
        Timeout(double seconds);
    };

} // namespace Events
} // namespace LSWE