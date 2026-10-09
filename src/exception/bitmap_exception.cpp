#include <LSWE/exception/bitmap_exception.hpp>

namespace LSWE {
namespace Utility {

    BitmapException::BitmapException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Utility
} // namespace LSWE