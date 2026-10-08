#include <LSWE/exception/mixer_exception.hpp>

namespace LSWE {
namespace Utility {

    MixerException::MixerException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Utility
} // namespace LSWE