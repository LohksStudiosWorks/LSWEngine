#include <LSWE/exception/mixer_exception.hpp>

namespace LSWE {
namespace Exception {

    MixerException::MixerException(const char* message) noexcept 
        : std::runtime_error(message)
    {}

} // namespace Exception
} // namespace LSWE