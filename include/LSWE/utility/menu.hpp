#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <memory>
#include <vector>
#include <span>

namespace LSWE {
namespace Utility {

    class Menu {
        class submenu;
    public:
        enum class type : uint8_t { BAR, POPUP };
        enum class flags : uint8_t { 
            DEFAULT = 0,
            DISABLED = ALLEGRO_MENU_ITEM_DISABLED,
            CHECKBOX = ALLEGRO_MENU_ITEM_CHECKBOX,
            CHECKED = ALLEGRO_MENU_ITEM_CHECKED
        };

        class element {
        public:
            static element create_item(std::string caption, uint16_t id, Menu::flags flags = Menu::flags::DEFAULT);
            static element create_div_item();
            static element create_submenu(std::string caption, Menu::flags flags = Menu::flags::DEFAULT);

            std::string get_caption() const;
            Menu::flags get_flags() const;
            int get_pos() const;
            uint16_t get_id() const;

            void set_caption(std::string caption);
            void set_flags(Menu::flags flags);

            void destroy_self();
            bool destroy_this_submenu();

            bool has_menu() const;
            submenu& as_menu();
        private:
            element(std::string caption, uint16_t id, Menu::flags flags, bool make_menu);
            element();

            friend class submenu;

            std::string caption;
            int pos;
            uint16_t id;
            Menu::flags flags;

            ALLEGRO_MENU* self_menu;
            submenu* self_menu_listing;
            std::shared_ptr<submenu> sub_menu;
        };

        Menu(const Menu&) = delete;
        void operator=(const Menu&) = delete;

        static Menu create(type type);

        Menu clone_as(type type);

        submenu& interface();
        submenu* operator->();

        element* find_by_event(const ALLEGRO_EVENT& ev);

        operator ALLEGRO_MENU*() const;

        ALLEGRO_EVENT_SOURCE* get_event_source() const;
        operator ALLEGRO_EVENT_SOURCE*() const;
    private:
        Menu(ALLEGRO_MENU* menu_base, type type);

        static element* find_recursive(submenu& menu, uint16_t id);

        class submenu {
        public:
            element& operator[](size_t pos);
            submenu& push(element element);
            submenu& push(submenu menu_element);
            bool remove(size_t pos);
            size_t size() const;

            std::vector<element>::iterator begin();
            std::vector<element>::iterator end();
        private:
            submenu(ALLEGRO_MENU* self, element* parent);

            friend class Menu;
            friend class element;

            ALLEGRO_MENU* self_menu;
            element* parent = nullptr;
            std::vector<element> elements;
        };

        std::shared_ptr<ALLEGRO_MENU> m_menu;
        submenu m_menu_interface;

        ALLEGRO_EVENT_SOURCE* m_menu_event_source;
        const type m_type;
    };

	constexpr Menu::flags operator|(const Menu::flags& a, const Menu::flags& b) { return static_cast<Menu::flags>(static_cast<std::underlying_type_t<Menu::flags>>(a) | static_cast<std::underlying_type_t<Menu::flags>>(b)); }
	constexpr Menu::flags operator&(const Menu::flags& a, const Menu::flags& b) { return static_cast<Menu::flags>(static_cast<std::underlying_type_t<Menu::flags>>(a) & static_cast<std::underlying_type_t<Menu::flags>>(b)); }

    
} // namespace Utility
} // namespace LSWE