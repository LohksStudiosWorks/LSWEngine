#pragma once

#include <allegro5/allegro5.h>

namespace LSWE {
namespace Graphics {
    
    class MonitorInfo {
    public:
        MonitorInfo(int adapter = ALLEGRO_DEFAULT_DISPLAY_ADAPTER);

        static int get_num_display_adapters();

        int get_width() const;
        int get_height() const;
        int get_display_adapter_index() const;
        int get_dpi() const;
        int get_refresh_rate() const;
    private:
        ALLEGRO_MONITOR_INFO m_info;
		int m_display_adapter = -1;
		int m_dpi = 0;
		int m_refresh_rate = 0;
    };

} // namespace Graphics
} // namespace LSWE