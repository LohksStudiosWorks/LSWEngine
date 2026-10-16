#include <LSWE/exception/menu_exception.hpp>

namespace LSWE {
namespace Exception {

    MenuException::MenuException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Exception
} // namespace LSWEe