#include <LSWE/exception/utility_exception.hpp>

namespace LSWE {
namespace Utility {

    UtilityException::UtilityException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace LSWE
} // namespace Utility