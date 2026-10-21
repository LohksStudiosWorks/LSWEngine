#pragma once

#include <allegro5/allegro5.h>

#include <vector>

namespace LSWE {
namespace Graphics {
    
    class DisplayModes {
    public:
        DisplayModes();

        const std::vector<ALLEGRO_DISPLAY_MODE> get() const;
    private:
		std::vector<ALLEGRO_DISPLAY_MODE> m_modes;
    };

} // namespace Graphics
} // namespace LSWE