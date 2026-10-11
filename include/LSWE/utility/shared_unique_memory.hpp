#pragma once

#include <memory>
#include <functional>

namespace LSWE {
namespace Utility {

    template<typename T>
    class SharedUniquePtr {
    public:
        SharedUniquePtr(T* ptr, std::function<void(T*)> deleter = std::default_delete<T>());

        T* operator->();
        operator T*();
        T* operator->() const;
        operator T*() const;

        T* get();
        T* get() const;

        T* release();
        void reset(T* new_ptr = nullptr);

        operator bool() const;
    private:
        struct shared_instance {
            T* m_instance = nullptr;
            std::function<void(T*)> m_deleter;

            shared_instance(T* ptr, std::function<void(T*)> dtr);
            ~shared_instance();
        };

        std::shared_ptr<shared_instance> m_shared;
    };

} // namespace Utility
} // namespace LSWE

#include <LSWE/utility/impl/shared_unique_memory.ipp>