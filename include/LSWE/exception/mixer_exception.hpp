#pragma once

#include <stdexcept>

namespace LSWE {
namespace Exception {

    class MixerException : public std::runtime_error {
    public:
        MixerException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Exception
} // namespace LSWE