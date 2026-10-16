#pragma once

#include <stdexcept>

namespace LSWE {
namespace Exception {

    class MenuException : public std::runtime_error {
    public:
        MenuException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Exception
} // namespace LSWE