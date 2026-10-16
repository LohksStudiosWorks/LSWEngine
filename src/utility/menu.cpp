#include <LSWE/utility/menu.hpp>

#include <limits>
#include <string>
#include <algorithm>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    std::string Menu::MenuItem::get_caption() const {
        if (m_id == static_cast<uint16_t>(-1) || m_pos < 0) return {};
        const char* str = al_get_menu_item_caption(m_this_menu, - m_pos);
        return str ? std::string(str) : std::string{};
    }

    Menu::flags Menu::MenuItem::get_flags() const {
        if (m_pos < 0) return Menu::flags::DEFAULT;
        return static_cast<Menu::flags>(al_get_menu_item_flags(m_this_menu, - m_pos));
    }

    int Menu::MenuItem::get_pos() const {
        if (m_pos < 0) return -1;
        return m_pos;
    }

    uint16_t Menu::MenuItem::get_id() const {
        if (m_pos < 0) return static_cast<uint16_t>(-1);
        return m_id;
    }
    bool Menu::MenuItem::has_child_menu() const {
        if (m_pos < 0) return false;
        return al_get_menu_item_submenu(m_this_menu, - m_pos) != nullptr;
    }

    Menu Menu::MenuItem::get_parent_menu() const {
        return Menu(m_this_menu, false);
    }

    Menu Menu::MenuItem::get_child_menu() const {
        if (m_pos < 0) throw 1;
        ALLEGRO_MENU* submenu = al_get_menu_item_submenu(m_this_menu, - m_pos);
        return Menu(submenu, false);
    }

    bool Menu::MenuItem::set_caption(std::string caption) {
        if (m_pos < 0 || m_id == static_cast<uint16_t>(-1)) return false;
        al_set_menu_item_caption(m_this_menu, - m_pos, caption.empty() ? nullptr : caption.c_str());
        return true;
    }

    bool Menu::MenuItem::set_flags(Menu::flags flags) {
        if (m_pos < 0 || m_id == static_cast<uint16_t>(-1)) return false;
        al_set_menu_item_flags(m_this_menu, - m_pos, static_cast<int>(flags));
        return true;
    }

    void Menu::MenuItem::self_remove() {
        if (m_pos < 0) return;
        al_remove_menu_item(m_this_menu, - m_pos);
        m_pos = -1;
    }

    Menu::MenuItem::MenuItem(ALLEGRO_MENU* self, int pos, uint16_t id)
        : m_this_menu(self), m_pos(pos), m_id(id)
    {}

    MenuBuilder Menu::MenuItem::export_builder() const {
        if (m_id == static_cast<uint16_t>(-1))
            return MenuBuilder();

        if (has_child_menu()) {
            return MenuBuilder(get_caption(), get_child_menu().export_builder_items(), get_flags());
        }
        else {
            return MenuBuilder(get_caption(), get_id(), get_flags());
        }
    }

    Menu Menu::create(type type) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();
        SingletonOf<AllegroInitMouse>::instance().setup();

        return Menu(type == type::BAR
            ? al_create_menu()
            : al_create_popup_menu(),
            true
        );
    }

    Menu Menu::copy(const Menu& copy_from, type type) {
        auto base = Menu(type == type::BAR
            ? al_create_menu()
            : al_create_popup_menu(),
            true
        );
        auto exported = copy_from.export_builder();

        printf("DEBUG\n%s\n", exported.dump().c_str());

        import_from_builder(base, exported);
        return base;
    }

    Menu Menu::build_from(type type, MenuBuilder builder) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();
        SingletonOf<AllegroInitMouse>::instance().setup();

        auto base = Menu(type == type::BAR
            ? al_create_menu()
            : al_create_popup_menu(),
            true
        );
        import_from_builder(base, builder);
        return base;
    }

    Menu Menu::clone() const {
        return build_from(
            al_is_menu_popup(m_this_menu) ? Menu::type::POPUP : Menu::type::BAR,
            export_builder()
        );
    }

    bool Menu::is_popup() const {
        return m_this_menu ? al_is_menu_popup(m_this_menu) : false;
    }

    void Menu::destroy() {
        if (m_base_menu.use_count() == 0) return;
        auto root = m_base_menu.lock();
        root.reset();
    }

    Menu Menu::push_item(std::string caption, uint16_t id, Menu::flags flags) {
        const int pos = al_append_menu_item(
            m_this_menu,
            caption.empty() ? nullptr : caption.c_str(),
            id,
            static_cast<int>(flags),
            nullptr,
            nullptr
        );

        return Menu(m_this_menu, false, m_base_menu);
    }

    Menu Menu::push_div() {
        const int pos = al_append_menu_item(
            m_this_menu,
            nullptr,
            static_cast<uint16_t>(-1),
            0,
            nullptr,
            nullptr
        );

        return Menu(m_this_menu, false, m_base_menu);
    }

    Menu Menu::push_new_menu(std::string caption, Menu::flags flags) {
        ALLEGRO_MENU* new_menu = al_create_menu();

        const int pos = al_append_menu_item(
            m_this_menu,
            caption.empty() ? nullptr : caption.c_str(),
            0,
            static_cast<int>(flags),
            nullptr,
            new_menu
        );
        
        return Menu(new_menu, false, m_base_menu);
    }

    Menu::MenuItem Menu::operator[](int pos) const {
        if (al_get_menu_item_flags(m_this_menu, - pos) < 0) throw 1;

        return MenuItem(m_this_menu, pos, al_get_menu_item_id(m_this_menu, - pos));
    }

    Menu::MenuItem Menu::operator()(uint16_t id) const {
        if (al_get_menu_item_flags(m_this_menu, id) < 0) throw 1;
        
        return MenuItem(m_this_menu, al_get_menu_item_pos(m_this_menu, id), id);
    }

    MenuBuilder Menu::export_builder() const {
        const auto submenus = export_builder_items();
        
        if (submenus.size() > 0)
            return MenuBuilder("", submenus, Menu::flags::DEFAULT);
        else
            return MenuBuilder("", 0, Menu::flags::DEFAULT);
    }
    
    std::optional<Menu::MenuItem> Menu::find_from_event(const ALLEGRO_EVENT& ev) const {
        if (ev.type != ALLEGRO_EVENT_MENU_CLICK) return std::nullopt;
        const uint16_t search = static_cast<uint16_t>(ev.user.data1);
        
        ALLEGRO_MENU* menu;
        int idx;

        if (al_find_menu_item(m_this_menu, search, &menu, &idx))
            return Menu::MenuItem(menu, idx, search);
        
        return std::nullopt;
    }

    Menu::operator ALLEGRO_MENU*() const {
        return m_this_menu;
    }

    ALLEGRO_EVENT_SOURCE* Menu::get_event_source() const {
        return al_enable_menu_event_source(m_this_menu);
    }

    Menu::operator ALLEGRO_EVENT_SOURCE*() const {
        return al_enable_menu_event_source(m_this_menu);
    }

    Menu::Menu(ALLEGRO_MENU* menu, bool handle_shared)
        : m_root_menu(handle_shared ? menu : nullptr, handle_shared ? al_destroy_menu : [](ALLEGRO_MENU*){}),
          m_base_menu(m_root_menu),
          m_this_menu(menu)
    {
        if (!m_this_menu) throw 1;
    }

    Menu::Menu(ALLEGRO_MENU* menu, bool handle_shared, WeakUniquePtr<ALLEGRO_MENU> parent)
        : m_root_menu(handle_shared ? menu : nullptr, al_destroy_menu),
          m_base_menu(parent),
          m_this_menu(menu)
    {
        if (!m_this_menu) throw 1;
    }
    
    void Menu::import_from_builder(Menu& menu, const MenuBuilder& builder) {
        for(const auto& child : builder.get_submenus()) {
            if (child.get_caption().empty() || child.get_id() == static_cast<uint16_t>(-1)) { // div
                menu.push_div();
            }
            else if (child.get_submenus().size() > 0) { // menu
                auto new_menu = menu.push_new_menu(child.get_caption(), child.get_flags());
                import_from_builder(new_menu, child);
            }
            else { // common item
                menu.push_item(child.get_caption(), child.get_id(), child.get_flags());
            }
        }
    }

    std::vector<MenuBuilder> Menu::export_builder_items() const {
        std::vector<MenuBuilder> submenus;

        for(int pos = 0; pos < std::numeric_limits<uint16_t>::max(); ++pos) {
            if (al_get_menu_item_flags(m_this_menu, - pos) < 0) break;
            MenuItem item = this->operator[](pos);
            submenus.push_back(item.export_builder());
        }

        return submenus;
    }


    MenuBuilder::MenuBuilder(std::string caption, uint16_t id, Menu::flags flags) 
        : caption(caption), id(id), flags(flags), submenus()
    {}

    MenuBuilder::MenuBuilder() 
        : caption({}), id(static_cast<uint16_t>(-1)), flags(Menu::flags::DEFAULT), submenus()
    {}

    MenuBuilder::MenuBuilder(std::string caption, std::vector<MenuBuilder> submenus, Menu::flags flags) 
        : caption(caption), id(0), flags(flags), submenus(submenus)
    {}

    std::string MenuBuilder::dump() const {
        std::string self = "{" \
            "\"caption\":" + (caption.empty() ? "null" : ("\"" + caption + "\"")) + "," \
            "\"id\":" + std::to_string(id) + "," \
            "\"flags\":" + std::to_string(static_cast<int>(flags)) + "," \
            "\"children\":[";

        for(const auto& child : submenus)
            self += child.dump() + ",";
        if (submenus.size() > 0) self.pop_back();

        self += "]}";
        return self;
    }

    std::string MenuBuilder::get_caption() const {
        return caption.c_str();
    }

    uint16_t MenuBuilder::get_id() const {
        return id;
    }

    Menu::flags MenuBuilder::get_flags() const {
        return flags;
    }

    const std::vector<MenuBuilder>& MenuBuilder::get_submenus() const {
        return submenus;
    }

    MenuBuilder& MenuBuilder::set_caption(std::string caption) {
        if (!caption.empty()) {
            if (id == static_cast<uint16_t>(-1)) id = 0;
            this->caption = caption;
        }
        else {
            this->caption.clear();
        }
        return *this;
    }

    MenuBuilder& MenuBuilder::set_id(uint16_t id) {
        this->id = id;
        return *this;
    }

    MenuBuilder& MenuBuilder::set_flags(Menu::flags flags) {
        this->flags = flags;
        return *this;
    }

    MenuBuilder& MenuBuilder::set_submenus(std::vector<MenuBuilder> submenus) {
        this->submenus = submenus;
        return *this;
    }
    
} // namespace Utility
} // namespace LSWE