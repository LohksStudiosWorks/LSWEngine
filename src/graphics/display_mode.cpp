#include <LSWE/graphics/display_mode.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Graphics {

    DisplayModes::DisplayModes() {
        Utility::SingletonOf<Utility::AllegroInit>().instance().setup();

		for (int ix = 0; ix < al_get_num_display_modes(); ++ix) {
			ALLEGRO_DISPLAY_MODE mode;
			al_get_display_mode(ix, &mode);
			m_modes.push_back(std::move(mode));
		}
    }

    const std::vector<ALLEGRO_DISPLAY_MODE> DisplayModes::get() const {
        return m_modes;
    }
    
} // namespace Graphics
} // namespace LSWE