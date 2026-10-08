#include <LSWE/exception/file_exception.hpp>

namespace LSWE {
namespace Utility {

    FileException::FileException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Utility
} // namespace LSWE