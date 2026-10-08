#pragma once

#include <stdexcept>

namespace LSWE {
namespace Utility {

    class UtilityException : public std::runtime_error {
    public:
        UtilityException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Utility
} // namespace LSWE