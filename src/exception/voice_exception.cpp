#include <LSWE/exception/voice_exception.hpp>

namespace LSWE {
namespace Utility {

    VoiceException::VoiceException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Utility
} // namespace LSWE