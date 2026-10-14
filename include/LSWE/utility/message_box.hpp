#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

namespace LSWE {
namespace Utility {

    int create_message_box(char const *title, char const *heading, char const *text, int flags = 0, char const *buttons = nullptr);
    
} // namespace Utility
} // namespace LSWE