#include <LSWE/graphics/monitor_info.hpp>

#include <LSWE/utility/startup.hpp>

namespace LSWE {
namespace Graphics {

    MonitorInfo::MonitorInfo(int adapter) {
        Utility::SingletonOf<Utility::AllegroInit>().instance().setup();

		if (adapter < 0) adapter = 0;
		if (adapter >= al_get_num_video_adapters()) adapter = al_get_num_video_adapters() - 1;

		int _old_targ = al_get_new_display_adapter();
		al_set_new_display_adapter(adapter);

		al_get_monitor_info(adapter, &m_info);
		m_display_adapter = al_get_new_display_adapter();
		m_dpi = al_get_monitor_dpi(adapter);
		m_refresh_rate = al_get_new_display_refresh_rate();

		al_set_new_display_adapter(_old_targ);
    }

    int MonitorInfo::get_num_display_adapters() {
        return al_get_num_video_adapters();
    }

    int MonitorInfo::get_width() const {
		return std::abs(m_info.x2 - m_info.x1);
    }

    int MonitorInfo::get_height() const {
		return std::abs(m_info.y2 - m_info.y1);
    }

    int MonitorInfo::get_display_adapter_index() const {
        return m_display_adapter;
    }

    int MonitorInfo::get_dpi() const {
        return m_dpi;
    }

    int MonitorInfo::get_refresh_rate() const {
        return m_refresh_rate;
    }

    
} // namespace Graphics
} // namespace LSWE