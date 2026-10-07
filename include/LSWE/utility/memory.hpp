#pragma once

#include <memory>

namespace LSWE {
namespace Utility {

    template<typename T>
    class LazyPointer {
    public:
        LazyPointer(const LazyPointer&) = delete;
        void operator=(const LazyPointer&) = delete;

        LazyPointer(LazyPointer&& oth);
        LazyPointer(T*&& ptr, void (*destroyer)(T*));
        LazyPointer() = default;

        ~LazyPointer();

        void operator=(LazyPointer&& oth);

        void reset(T*&& ptr = nullptr, void (*destroyer)(T*) = nullptr);

        T* get() const;

        T* operator->();
        const T* operator->() const;
    private:
        T* m_raw{};
        void (*m_destroy)(T*){};
    };
    
} // namespace LSWE
} // namespace Utility

#include <LSWE/utility/impl/memory.ipp>