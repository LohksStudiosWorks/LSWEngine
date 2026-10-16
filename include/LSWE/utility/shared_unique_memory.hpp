#pragma once

#include <memory>
#include <functional>

namespace LSWE {
namespace Utility {


    template<typename T>
    class WeakUniquePtr;

    template<typename T>
    class SharedUniquePtr {
    public:
        SharedUniquePtr(T* ptr, std::function<void(T*)> deleter = std::default_delete<T>());

        SharedUniquePtr& operator=(T* ptr);
        SharedUniquePtr& operator=(const SharedUniquePtr& shr);

        T* operator->();
        operator T*();
        T* operator->() const;
        operator T*() const;

        T* get();
        T* get() const;

        T* release();
        void reset(T* new_ptr = nullptr);

        size_t use_count() const;

        operator bool() const;
    private:
        struct shared_instance {
            T* m_instance = nullptr;
            std::function<void(T*)> m_deleter;

            shared_instance(T* ptr, std::function<void(T*)> dtr);
            ~shared_instance();
        };

        SharedUniquePtr(std::shared_ptr<shared_instance>&& instance);

        friend class WeakUniquePtr<T>;

        std::shared_ptr<shared_instance> m_shared;
    };

    template<typename T>
    class WeakUniquePtr {
    public:
        WeakUniquePtr() = default;
        WeakUniquePtr(const WeakUniquePtr<T>& oth);
        WeakUniquePtr(const SharedUniquePtr<T>& oth);

        WeakUniquePtr& operator=(const WeakUniquePtr<T>& oth);
        WeakUniquePtr& operator=(const SharedUniquePtr<T>& oth);

        SharedUniquePtr<T> lock();

        void reset();

        bool expired() const;

        size_t use_count() const;
    private:
        std::weak_ptr<typename SharedUniquePtr<T>::shared_instance> m_weak;
    };



} // namespace Utility
} // namespace LSWE

#include <LSWE/utility/impl/shared_unique_memory.ipp>