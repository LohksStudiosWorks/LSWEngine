#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Exception {

    NullException::NullException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Exception
} // namespace LSWE