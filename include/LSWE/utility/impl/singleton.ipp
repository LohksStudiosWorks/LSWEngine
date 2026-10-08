#include <typeinfo>

namespace LSWE {
namespace Utility {

    template<typename T>
    struct SingletonOf<T>::wrap_constructor {
        T instance;
        wrap_constructor() {
            SingletonInfo::push_name(typeid(T).name());
        }
    };

    template<typename T>
    inline T& SingletonOf<T>::instance() {
        static wrap_constructor static_ref; // std: no need for mutex
        return static_ref.instance;
    }

    template<typename T>
    inline T* SingletonOf<T>::operator->() const {
        return &instance();
    }
    
} // namespace Utility
} // namespace LSWE