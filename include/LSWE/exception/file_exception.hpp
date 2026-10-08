#pragma once

#include <stdexcept>

namespace LSWE {
namespace Utility {

    class FileException : public std::runtime_error {
    public:
        FileException(const char* message) noexcept;
        using std::runtime_error::what;
    };

} // namespace Utility
} // namespace LSWE