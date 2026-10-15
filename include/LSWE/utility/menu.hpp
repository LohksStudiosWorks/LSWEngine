#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace LSWE {
namespace Utility {

    class MenuBuilder;

    class Menu {
    public:
        enum class type : uint8_t { BAR, POPUP };
        enum class flags : uint8_t { 
            DEFAULT = 0,
            DISABLED = ALLEGRO_MENU_ITEM_DISABLED,
            CHECKBOX = ALLEGRO_MENU_ITEM_CHECKBOX,
            CHECKED = ALLEGRO_MENU_ITEM_CHECKED
        };

        static Menu create(type type);
        static Menu copy(const Menu& copy_from, type type);
        static Menu build_from(type type, MenuBuilder builder);

        std::string get_caption() const;
        Menu::flags get_flags() const;
        int get_pos() const;
        uint16_t get_id() const;
        bool set_caption(std::string caption);
        bool set_flags(Menu::flags flags);

        Menu push_item(std::string caption, uint16_t id, Menu::flags flags = Menu::flags::DEFAULT);
        Menu push_div();
        Menu push_new_menu(std::string caption, Menu::flags flags = Menu::flags::DEFAULT);

        Menu operator[](int pos) const;
        Menu operator()(uint16_t id) const;

        MenuBuilder export_builder() const;

        std::optional<Menu> find_from_event(const ALLEGRO_EVENT& ev);
        
        operator ALLEGRO_MENU*() const;
        ALLEGRO_EVENT_SOURCE* get_event_source() const;
        operator ALLEGRO_EVENT_SOURCE*() const;
    protected:
        // base menu:
        Menu(ALLEGRO_MENU* menu);
        // children menu:
        Menu(ALLEGRO_MENU* self_menu, int pos, uint16_t id);

        ALLEGRO_MENU* get_menu() const;
        // adds submenu in it
        ALLEGRO_MENU* get_or_recreate_as_menu();
        // remove submenu in it
        void recreate_as_item();

        static void import_from_builder(Menu& menu, const MenuBuilder& builder);

        std::optional<Menu> find_recursive(uint16_t by_id);

        std::shared_ptr<ALLEGRO_MENU> m_root_menu;
        ALLEGRO_MENU* m_this_menu;

        int m_pos; // -1 == base menu.
        const uint16_t m_id; // -1 == div
    };

    class MenuBuilder {
    public:
        MenuBuilder(std::string caption, uint16_t id, Menu::flags flags = Menu::flags::DEFAULT);
        MenuBuilder();
        MenuBuilder(std::string caption, std::vector<MenuBuilder> submenus, Menu::flags flags = Menu::flags::DEFAULT);

        std::string dump() const;

        std::string get_caption() const;
        uint16_t get_id() const;
        Menu::flags get_flags() const;
        const std::vector<MenuBuilder>& get_submenus() const;

        MenuBuilder& set_caption(std::string caption);
        MenuBuilder& set_id(uint16_t id);
        MenuBuilder& set_flags(Menu::flags flags);
        MenuBuilder& set_submenus(std::vector<MenuBuilder> submenus);
    private:
        std::vector<MenuBuilder> submenus;
        std::string caption;
        uint16_t id;
        Menu::flags flags;
    };
    
} // namespace Utility

constexpr Utility::Menu::flags operator|(const Utility::Menu::flags& a, const Utility::Menu::flags& b) {
    return static_cast<Utility::Menu::flags>(
        static_cast<std::underlying_type_t<Utility::Menu::flags>>(a) | static_cast<std::underlying_type_t<Utility::Menu::flags>>(b)
    );
}
constexpr Utility::Menu::flags operator&(const Utility::Menu::flags& a, const Utility::Menu::flags& b) {
    return static_cast<Utility::Menu::flags>(
        static_cast<std::underlying_type_t<Utility::Menu::flags>>(a) & static_cast<std::underlying_type_t<Utility::Menu::flags>>(b)
    );
}

} // namespace LSWE