#include <LSWE/exception/voice_exception.hpp>

namespace LSWE {
namespace Exception {

    VoiceException::VoiceException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Exception
} // namespace LSWE