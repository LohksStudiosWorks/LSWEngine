#include <LSWE/utility/file_chooser.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    FileChooser FileChooser::create(const std::string& title, const std::string& patterns_mime_semi_colon, const std::string& initial_path, int mode) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();

        return FileChooser(al_create_native_file_dialog(
            initial_path.length() > 0 ? initial_path.c_str() : nullptr,
            title.c_str(),
            patterns_mime_semi_colon.c_str(),
            mode
        ));
    }
    
    std::vector<std::string> FileChooser::request() {
        if (!al_show_native_file_dialog(nullptr, m_choose.get())) 
            return {};
        
        int count = al_get_native_file_dialog_count(m_choose.get());
        if (count <= 0) return {};

        std::vector<std::string> chosen;
        for(int p = 0; p < count; ++p) {
            chosen.push_back(std::string(al_get_native_file_dialog_path(m_choose.get(), static_cast<size_t>(p))));
        }
        return chosen;
    }

    FileChooser::FileChooser(ALLEGRO_FILECHOOSER* choose)
        : m_choose(choose, al_destroy_native_file_dialog)
    {
        if (!m_choose)
            throw Exception::NullException("Filechooser was null");
    }

} // namespace Utility
} // namespace LSWE