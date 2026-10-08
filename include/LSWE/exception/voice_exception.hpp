#pragma once

#include <stdexcept>

namespace LSWE {
namespace Utility {

    class VoiceException : public std::runtime_error {
    public:
        VoiceException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Utility
} // namespace LSWE