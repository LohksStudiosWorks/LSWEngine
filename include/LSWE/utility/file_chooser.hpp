#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <memory>
#include <vector>
#include <string>

namespace LSWE {
namespace Utility {

    class FileChooser {
    public:
        static FileChooser create(const std::string& title, const std::string& patterns_mime_semi_colon, const std::string& initial_path = "", int mode = 0);

        std::vector<std::string> request();
    private:
        FileChooser(ALLEGRO_FILECHOOSER* choose);

        std::shared_ptr<ALLEGRO_FILECHOOSER> m_choose;
    };
    
} // namespace Utility
} // namespace LSWE