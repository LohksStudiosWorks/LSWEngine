#include <LSWE/exception/utility_exception.hpp>

namespace LSWE {
namespace Exception {

    UtilityException::UtilityException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Exception
} // namespace LSWE