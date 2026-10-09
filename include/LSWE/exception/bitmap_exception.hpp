#pragma once

#include <stdexcept>

namespace LSWE {
namespace Utility {

    class BitmapException : public std::runtime_error {
    public:
        BitmapException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Utility
} // namespace LSWE