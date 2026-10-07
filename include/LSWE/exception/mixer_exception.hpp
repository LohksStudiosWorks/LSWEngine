#pragma once

#include <stdexcept>

namespace LSWE {
namespace Utility {

    class MixerException : public std::runtime_error {
    public:
        MixerException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace LSWE
} // namespace Utility