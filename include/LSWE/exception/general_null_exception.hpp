#pragma once

#include <stdexcept>

namespace LSWE {
namespace Exception {

    class NullException : public std::runtime_error {
    public:
        NullException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Exception
} // namespace LSWE