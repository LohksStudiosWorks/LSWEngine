#include <LSWE/utility/menu.hpp>

#include <limits>
#include <string>
#include <algorithm>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {

    Menu Menu::create(type type) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();
        SingletonOf<AllegroInitMouse>::instance().setup();

        return Menu(type == type::BAR
            ? al_create_menu()
            : al_create_popup_menu()
        );
    }

    Menu Menu::copy(const Menu& copy_from, type type) {
        auto base = Menu(type == type::BAR
            ? al_create_menu()
            : al_create_popup_menu()
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
            : al_create_popup_menu()
        );
        import_from_builder(base, builder);
        return base;
    }

    std::string Menu::get_caption() const {
        if (m_id == static_cast<uint16_t>(-1) || m_pos < 0) return {};
        const char* str = al_get_menu_item_caption(m_this_menu, - m_pos);
        
        return str ? std::string(str) : std::string{};
    }

    Menu::flags Menu::get_flags() const {
        return static_cast<Menu::flags>(al_get_menu_item_flags(m_this_menu, - m_pos));
    }

    int Menu::get_pos() const {
        if (m_pos < 0) return -1;
        return m_pos;
    }

    uint16_t Menu::get_id() const {
        if (m_pos < 0) return static_cast<uint16_t>(-1);
        return m_id;
    }

    bool Menu::set_caption(std::string caption) {
        if (m_pos < 0 || m_id == static_cast<uint16_t>(-1)) return false;
        al_set_menu_item_caption(m_this_menu, - m_pos, caption.empty() ? nullptr : caption.c_str());
        return true;
    }

    bool Menu::set_flags(Menu::flags flags) {
        if (m_pos < 0 || m_id == static_cast<uint16_t>(-1)) return false;
        al_set_menu_item_flags(m_this_menu, - m_pos, static_cast<int>(flags));
        return true;
    }

    Menu Menu::push_item(std::string caption, uint16_t id, Menu::flags flags) {
        ALLEGRO_MENU* children_menu = get_or_recreate_as_menu();

        const int pos = al_append_menu_item(children_menu, caption.empty() ? nullptr : caption.c_str(), id, static_cast<int>(flags), nullptr, nullptr);

        return Menu(children_menu, pos, id);
    }

    Menu Menu::push_div() {
        ALLEGRO_MENU* children_menu = get_or_recreate_as_menu();

        const int pos = al_append_menu_item(children_menu, nullptr, static_cast<uint16_t>(-1), 0, nullptr, nullptr);

        return Menu(children_menu, pos, static_cast<uint16_t>(-1));
    }

    Menu Menu::push_new_menu(std::string caption, Menu::flags flags) {
        ALLEGRO_MENU* children_menu = get_or_recreate_as_menu();
        
        ALLEGRO_MENU* new_menu = al_create_menu();
        const int pos = al_append_menu_item(children_menu, caption.empty() ? nullptr : caption.c_str(), 0, static_cast<int>(flags), nullptr, new_menu);
        
        return Menu(children_menu, pos, 0);
    }

    Menu Menu::operator[](int pos) const {
        ALLEGRO_MENU* children_menu = get_menu();
        if (!children_menu) throw 1;
        if (al_get_menu_item_flags(children_menu, - pos) < 0) throw 1;

        return Menu(children_menu, pos, al_get_menu_item_id(children_menu, - pos));
    }

    Menu Menu::operator()(uint16_t id) const {
        ALLEGRO_MENU* children_menu = get_menu();
        if (!children_menu) throw 1;
        if (al_get_menu_item_flags(children_menu, id) < 0) throw 1;
        
        return Menu(children_menu, al_get_menu_item_pos(children_menu, id), id);
    }

    MenuBuilder Menu::export_builder() const {
        if (m_id == static_cast<uint16_t>(-1))
            return MenuBuilder();
        
        std::vector<MenuBuilder> submenus;

        if (ALLEGRO_MENU* children_menu = get_menu(); children_menu) {
            for(int pos = 0; pos < std::numeric_limits<uint16_t>::max(); ++pos) {
                if (al_get_menu_item_flags(children_menu, - pos) < 0) break;
                Menu submenu = this->operator[](pos);
                submenus.push_back(submenu.export_builder());
            }
        }
        
        if (submenus.size() > 0)
            return MenuBuilder(get_caption(), submenus, get_flags());
        else
            return MenuBuilder(get_caption(), get_id(), get_flags());
    }
    
    std::optional<Menu> Menu::find_from_event(const ALLEGRO_EVENT& ev) {
        if (ev.type != ALLEGRO_EVENT_MENU_CLICK) return std::nullopt;
        const uint16_t search = static_cast<uint16_t>(ev.user.data1);

        return find_recursive(search);
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

    Menu::Menu(ALLEGRO_MENU* menu)
        : m_root_menu(menu, al_destroy_menu), m_this_menu(menu), m_pos(-1), m_id(0)
    {
        if (!m_this_menu) throw 1;
    }
    
    Menu::Menu(ALLEGRO_MENU* self_menu, int pos, uint16_t id)
        : m_this_menu(self_menu), m_pos(pos), m_id(id)
    {
        if (!m_this_menu) throw 1;
    }

    ALLEGRO_MENU* Menu::get_menu() const {
        if (m_pos < 0) return m_this_menu;

        return al_get_menu_item_submenu(m_this_menu, - m_pos);
    }

    ALLEGRO_MENU* Menu::get_or_recreate_as_menu() {
        if (m_pos < 0) return m_this_menu;

        ALLEGRO_MENU* submenu = al_get_menu_item_submenu(m_this_menu, - m_pos);
        if (submenu) return submenu;

        std::string caption = get_caption();
        Menu::flags flags = get_flags();

        submenu = al_create_menu();
        if (!submenu) throw 1;

        if (!al_remove_menu_item(m_this_menu, - m_pos)) throw 1;
        al_insert_menu_item(m_this_menu, - m_pos, caption.c_str(), 0, static_cast<int>(flags), nullptr, submenu);

        return submenu;
    }

    void Menu::recreate_as_item() {
        std::string caption = get_caption();
        Menu::flags flags = get_flags();
        
        if (!al_remove_menu_item(m_this_menu, - m_pos)) throw 1;
        al_insert_menu_item(m_this_menu, - m_pos, caption.c_str(), m_id, static_cast<int>(flags), nullptr, nullptr);
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

    std::optional<Menu> Menu::find_recursive(uint16_t by_id) {
        if (m_id == by_id) return *this;

        if (ALLEGRO_MENU* children_menu = get_menu(); children_menu) {
            for(int pos = 0; pos < std::numeric_limits<uint16_t>::max(); ++pos) {
                const int id = al_get_menu_item_id(children_menu, - pos);
                if (id < 0) break;

                if (static_cast<uint16_t>(id) == by_id)
                    return Menu(children_menu, al_get_menu_item_pos(children_menu, by_id), by_id);
            }
        }
        return std::nullopt;
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