#pragma once

#include <memory>
#include <functional>
#include <optional>

namespace LSWE {
namespace Utility {

    template<typename T>
    class LazyPointer {
    public:
        LazyPointer(const LazyPointer&) = delete;
        void operator=(const LazyPointer&) = delete;

        LazyPointer(LazyPointer&& oth);
        LazyPointer(T*&& ptr, std::function<void(T*)> destroyer);
        LazyPointer() = default;

        ~LazyPointer();

        void operator=(LazyPointer&& oth);

        void reset(T*&& ptr = nullptr, std::optional<std::function<void(T*)>> destroyer = std::nullopt);

        T* get() const;

        T* operator->();
        const T* operator->() const;
    private:
        T* m_raw{};
        std::function<void(T*)> m_destroy;
    };
    
} // namespace LSWE
} // namespace Utility

#include <LSWE/utility/impl/memory.ipp>