#pragma once

#include <stdexcept>

namespace LSWE {
namespace Exception {

    class BitmapException : public std::runtime_error {
    public:
        BitmapException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Exception
} // namespace LSWE