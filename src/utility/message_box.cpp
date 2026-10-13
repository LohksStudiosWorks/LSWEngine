#include <LSWE/utility/message_box.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    int create_message_box(char const *title, char const *heading, char const *text, int flags, char const *buttons) {
        // This message box does not need Allegro init at all.
        
        ALLEGRO_DISPLAY* disp = al_get_current_display();

        return al_show_native_message_box(disp,
            title, heading, text,
            buttons, flags);
    }

} // namespace Utility
} // namespace LSWE