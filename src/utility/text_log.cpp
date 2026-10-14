#include <LSWE/utility/text_log.hpp>

#include <sstream>
#include <stdarg.h>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    void TextLog::open(char const* title, int flags) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();

        m_textlog = std::shared_ptr<ALLEGRO_TEXTLOG>(al_open_native_text_log(title, flags), al_close_native_text_log);

        if (!m_textlog)
            throw Exception::NullException("TextLog is null");
    }

    void TextLog::close() {
        m_textlog.reset();
    }

	void TextLog::printf(const char* format, ...) {
        va_list args, cpy;

		va_start(args, format);
        va_copy(cpy, args);

        int size = vsnprintf(nullptr, 0, format, args);
        if (size <= 0) return;

        va_end(args);

        std::string buf(size + 1, '\0');
        vsnprintf(buf.data(), size + 1, format, cpy);

        va_end(cpy);

        al_append_native_text_log(m_textlog.get(), "%.*s\n", size, buf.c_str());
	}

    ALLEGRO_EVENT_SOURCE* TextLog::get_event_source() const {
        return m_textlog ? al_get_native_text_log_event_source(m_textlog.get()) : nullptr;
    }

    TextLog::operator ALLEGRO_EVENT_SOURCE*() const {
        return m_textlog ? al_get_native_text_log_event_source(m_textlog.get()) : nullptr;
    }    

} // namespace Utility
} // namespace LSWE