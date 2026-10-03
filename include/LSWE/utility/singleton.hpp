#pragma once

#include <mutex>
#include <vector>
#include <string_view>

namespace LSWE {
namespace Utility {

    class SingletonInfo;

    /**
     * @brief SingletonOf is a tool to easily allow for Singleton of a class
     * 
     * You can add `friend class Singleton<Configuration>;` to a class and make its constructor private to
     * require a Singleton of it to be used instead of instantiating it by hand 
     * 
     * @tparam T class to instantiate Singleton of
     */
    template<typename T>
    class SingletonOf {
    public:
        /**
         * @brief Get the singleton instance of the class
         * 
         * @return T& reference to the static singleton instance
         */
        static T& instance();

        /**
         * @brief If you desire to have a object-like ->, this allows for it
         * 
         * This may be useful like SingletonOf<Logger> log; ... log->info() ...
         * 
         * @return `T*` a pointer to the static singleton instance
         */
        T* operator->() const;
    private:
        friend class SingletonInfo;

        struct wrap_constructor;

        static T& get_static_ref();
    };

    class SingletonInfo {
    public:
        static const std::vector<std::string_view>& list_all();
    private:    
        template<typename T>
        friend class SingletonOf<T>::wrap_constructor;

        static void push_name(std::string_view name);

        static std::vector<std::string_view>& get();
    };

} // namespace LSWE
} // namespace Utility

#include <LSWE/utility/impl/singleton.ipp>