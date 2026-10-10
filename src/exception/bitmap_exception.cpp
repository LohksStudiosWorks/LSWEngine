#include <LSWE/exception/bitmap_exception.hpp>

namespace LSWE {
namespace Exception {

    BitmapException::BitmapException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Exception
} // namespace LSWE