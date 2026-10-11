
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    template<typename T>
    SharedUniquePtr<T>::shared_instance::shared_instance(T* ptr, std::function<void(T*)> dtr) 
        : m_instance(ptr), m_deleter(dtr)
    {
        if (!m_instance || !m_deleter)
            throw Exception::NullException("Pointer or destructor was null");
    }

    template<typename T>
    SharedUniquePtr<T>::shared_instance::~shared_instance() {
        if (m_instance)
            m_deleter(m_instance);
    }

    template<typename T>
    SharedUniquePtr<T>::SharedUniquePtr(T* ptr, std::function<void(T*)> deleter) 
        : m_shared(new shared_instance(ptr, deleter))
    {}

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
        return m_shared.get()->m_instance != nullptr;
    }
    
} // namespace Utility
} // namespace LSWE