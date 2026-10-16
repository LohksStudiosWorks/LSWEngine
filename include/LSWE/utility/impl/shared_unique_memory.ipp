
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    template<typename T>
    SharedUniquePtr<T>::SharedUniquePtr(T* ptr, std::function<void(T*)> deleter) 
        : m_shared(new shared_instance(ptr, deleter))
    {}

    template<typename T>
    SharedUniquePtr<T>& SharedUniquePtr<T>::operator=(T* ptr) {
        reset(ptr);
        return *this;
    }

    template<typename T>
    SharedUniquePtr<T>& SharedUniquePtr<T>::operator=(const SharedUniquePtr<T>& shr) {
        m_shared = shr.m_shared;
        return *this;
    }

    template<typename T>
    T* SharedUniquePtr<T>::operator->() {
        return m_shared.get()->m_instance;
    }

    template<typename T>
    SharedUniquePtr<T>::operator T*() {
        return m_shared.get()->m_instance;
    }

    template<typename T>
    T* SharedUniquePtr<T>::operator->() const {
        return m_shared.get()->m_instance;
    }
    
    template<typename T>
    SharedUniquePtr<T>::operator T*() const {
        return m_shared.get()->m_instance;
    }
    
    template<typename T>
    T* SharedUniquePtr<T>::get() {
        return m_shared.get()->m_instance;
    }

    template<typename T>
    T* SharedUniquePtr<T>::get() const {
        return m_shared.get()->m_instance;
    }
    
    template<typename T>
    T* SharedUniquePtr<T>::release() {
        T* ptr = m_shared.get()->m_instance;
        m_shared.get()->m_instance = nullptr;
        return ptr;
    }

    template<typename T>
    void SharedUniquePtr<T>::reset(T* new_ptr) {
        shared_instance* i = m_shared.get();
        if (i->m_instance) i->m_deleter(i->m_instance);
        i->m_instance = new_ptr;
    }

    template<typename T>
    SharedUniquePtr<T>::operator bool() const {
        return m_shared && m_shared.get()->m_instance != nullptr;
    }

    template<typename T>
    SharedUniquePtr<T>::SharedUniquePtr(std::shared_ptr<shared_instance>&& instance)
        : m_shared(std::move(instance))
    {
        if (!m_shared)
            throw Exception::NullException("Shared unique ptr was null");
    }

    template<typename T>
    SharedUniquePtr<T>::shared_instance::shared_instance(T* ptr, std::function<void(T*)> dtr) 
        : m_instance(ptr), m_deleter(dtr)
    {
        if (!m_deleter)
            throw Exception::NullException("Destructor was null");
    }

    template<typename T>
    SharedUniquePtr<T>::shared_instance::~shared_instance() {
        if (m_instance)
            m_deleter(m_instance);
    }

    template<typename T>
    WeakUniquePtr<T>::WeakUniquePtr(const WeakUniquePtr<T>& oth) 
        : m_weak(oth.m_weak)
    {}

    template<typename T>
    WeakUniquePtr<T>::WeakUniquePtr(const SharedUniquePtr<T>& oth) 
        : m_weak(oth.m_shared)
    {}

    template<typename T>
    WeakUniquePtr<T>& WeakUniquePtr<T>::operator=(const WeakUniquePtr<T>& oth) {
        m_weak = oth.m_weak;
        return *this;
    }

    template<typename T>
    WeakUniquePtr<T>& WeakUniquePtr<T>::operator=(const SharedUniquePtr<T>& oth) {
        m_weak = oth.m_shared;
        return *this;
    }

    template<typename T>
    SharedUniquePtr<T> WeakUniquePtr<T>::lock() {
        auto locked = m_weak.lock();
        if (!locked) throw Exception::NullException("Could not lock the weak pointer: null");
        return SharedUniquePtr<T>(std::move(locked));
    }

    template<typename T>
    void WeakUniquePtr<T>::reset() {
        m_weak.reset();
    }

    template<typename T>
    bool WeakUniquePtr<T>::expired() const {
        return m_weak.expired();
    }

    template<typename T>
    size_t WeakUniquePtr<T>::use_count() const {
        return m_weak.use_count();
    }

    
} // namespace Utility
} // namespace LSWE