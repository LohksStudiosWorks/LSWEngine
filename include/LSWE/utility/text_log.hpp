#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <memory>

#include <LSWE/utility/singleton.hpp>

namespace LSWE {
namespace Utility {

    class TextLog {
    public:
        void open(char const* title, int flags = 0);
        void close();

        void printf(const char* format, ...);

        ALLEGRO_EVENT_SOURCE* get_event_source() const;
        operator ALLEGRO_EVENT_SOURCE*() const;
    private:
        TextLog() = default;

        std::shared_ptr<ALLEGRO_TEXTLOG> m_textlog;

        friend class SingletonOf<TextLog>;
    };
    
} // namespace Utility
} // namespace LSWE