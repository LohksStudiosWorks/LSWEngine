#pragma once

#include <stdexcept>

namespace LSWE {
namespace Exception {

    class VoiceException : public std::runtime_error {
    public:
        VoiceException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Exception
} // namespace LSWE