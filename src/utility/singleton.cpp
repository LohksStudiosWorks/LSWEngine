#include <LSWE/utility/singleton.hpp>

namespace LSWE {
namespace Utility {

    const std::vector<std::string_view>& SingletonInfo::list_all() {
        return get();
    }

    void SingletonInfo::push_name(std::string_view name) {
        get().push_back(std::move(name));
    }

    std::vector<std::string_view>& SingletonInfo::get() {
        static std::vector<std::string_view> ref_static{};
        return ref_static;
    }

} // namespace Utility
} // namespace LSWE