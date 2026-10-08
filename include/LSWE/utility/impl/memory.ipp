#pragma once

#include <utility>

#include <LSWE/exception/utility_exception.hpp>

namespace LSWE {
namespace Utility {

    template<typename T>
    LazyPointer<T>::LazyPointer(LazyPointer&& oth) 
        : m_raw(std::exchange(oth.m_raw, nullptr)), m_destroy(std::exchange(oth.m_destroy, nullptr))
    {
        if (!m_destroy && m_raw)
            throw UtilityException("LazyPointer got pointer, but no destructor!");
    }

    template<typename T>
    LazyPointer<T>::LazyPointer(T*&& ptr, std::function<void(T*)> destroyer) 
        : m_raw(std::exchange(ptr, nullptr)), m_destroy(destroyer)
    {
        if (!m_destroy && m_raw)
            throw UtilityException("LazyPointer got pointer, but no destructor!");
    }

    template<typename T>
    LazyPointer<T>::~LazyPointer() {
        reset(nullptr, nullptr);
    }

    template<typename T>
    void LazyPointer<T>::operator=(LazyPointer&& oth) {
        reset(std::exchange(oth.m_raw, nullptr), std::exchange(oth.m_destroy, nullptr));
    }

    template<typename T>
    void LazyPointer<T>::reset(T*&& ptr, std::optional<std::function<void(T*)>> destroyer) {
        if (m_raw)
            m_destroy(m_raw);
        m_raw = std::exchange(ptr, nullptr);

        if (!destroyer.has_value() && m_raw)
            throw UtilityException("LazyPointer got pointer, but no destructor, on reset!");

        m_destroy = destroyer.value();
    }
    
    template<typename T>
    T* LazyPointer<T>::get() const {
        return m_raw;
    }

    template<typename T>
    T* LazyPointer<T>::operator->() {
        return m_raw;
    }

    template<typename T>
    const T* LazyPointer<T>::operator->() const {
        return m_raw;
    }

} // namespace LSWE
} // namespace Utility