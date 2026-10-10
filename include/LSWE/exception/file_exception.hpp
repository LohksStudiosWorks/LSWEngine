#pragma once

#include <stdexcept>

namespace LSWE {
namespace Exception {

    class FileException : public std::runtime_error {
    public:
        FileException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Exception
} // namespace LSWE