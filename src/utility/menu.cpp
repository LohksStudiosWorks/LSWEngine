#include <LSWE/utility/menu.hpp>

#include <limits>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Utility {
    
    /// === Menu Element class === ///

    Menu::element Menu::element::create_item(std::string caption, uint16_t id, Menu::flags flags) {
        return Menu::element(caption, id, flags, false);
    }

    Menu::element Menu::element::create_div_item() {
        return Menu::element();
    }

    Menu::element Menu::element::create_submenu(std::string caption, Menu::flags flags) {
        return Menu::element(caption, 0, flags, true);
    }

    std::string Menu::element::get_caption() const {
        return caption;
    }

    Menu::flags Menu::element::get_flags() const {
        return flags;
    }

    int Menu::element::get_pos() const {
        return pos;
    }

    uint16_t Menu::element::get_id() const {
        return id;
    }

    void Menu::element::set_caption(std::string caption) {
        this->caption = caption;
        al_set_menu_item_caption(self_menu, -pos, this->caption.c_str());
    }

    void Menu::element::set_flags(Menu::flags flags) {
        this->flags = flags;
        al_set_menu_item_flags(self_menu, -pos, static_cast<int>(this->flags));
    }

    void Menu::element::destroy_self() {
        self_menu_listing->remove(pos);
    }

    bool Menu::element::destroy_this_submenu() {
        if (!sub_menu) return false;
        al_destroy_menu(sub_menu->self_menu);
        sub_menu.reset();
        return true;
    }

    bool Menu::element::has_menu() const {
        return sub_menu.get() != nullptr;
    }

    Menu::submenu& Menu::element::as_menu() {
        if (!sub_menu) throw 1;
        return *sub_menu;
    }

    Menu::element::element(std::string caption, uint16_t id, Menu::flags flags, bool make_menu) 
        : caption(caption), id(id), flags(flags), self_menu(nullptr),
          sub_menu(std::shared_ptr<submenu>(make_menu ? new submenu(al_create_menu(), this) : nullptr))
    {
        if (id > std::numeric_limits<int16_t>::max()) throw 1;
    }

    Menu::element::element() 
        : caption({}), id(static_cast<uint16_t>(-1)), flags(Menu::flags::DEFAULT), self_menu(nullptr),
          sub_menu(std::shared_ptr<submenu>(nullptr))
    {}

    /// ===== Menu class ===== ///

    Menu Menu::create(Menu::type type) {
        SingletonOf<AllegroInit>::instance().setup();
        SingletonOf<AllegroInitDialog>::instance().setup();
        SingletonOf<AllegroInitMouse>::instance().setup();

        return Menu(type == type::BAR
            ? al_create_menu()
            : al_create_popup_menu(),
            type);
    }

    Menu Menu::clone_as(type type) {
        return Menu(type == type::BAR
            ? al_clone_menu(m_menu.get())
            : al_clone_menu_for_popup(m_menu.get()), type);
    }

    Menu::submenu& Menu::interface() {
        return m_menu_interface;
    }

    Menu::submenu* Menu::operator->() {
        return &m_menu_interface;
    }

    Menu::element* Menu::find_by_event(const ALLEGRO_EVENT& ev) {
        if (ev.type != ALLEGRO_EVENT_MENU_CLICK) return nullptr;
        return find_recursive(m_menu_interface, static_cast<uint16_t>(ev.user.data1));
    }

    Menu::operator ALLEGRO_MENU*() const {
        return m_menu.get();
    }

    ALLEGRO_EVENT_SOURCE* Menu::get_event_source() const {
        return m_menu_event_source;
    }

    Menu::operator ALLEGRO_EVENT_SOURCE*() const {
        return m_menu_event_source;
    }

    Menu::Menu(ALLEGRO_MENU* menu, type type)
        : m_menu(menu, al_destroy_menu),
          m_menu_interface(menu, nullptr),
          m_menu_event_source(al_enable_menu_event_source(menu)),
          m_type(type)
    {
        if (!m_menu || !m_menu_event_source)
            throw Exception::NullException("Menu was null");
    }

    Menu::element* Menu::find_recursive(submenu& menu, uint16_t id) {
        for(Menu::element& el : menu) {
            if (el.get_id() == id)
                return &el;

            if (!el.has_menu())
                continue;
                
            if (element* under_ptr = find_recursive(el.as_menu(), id); under_ptr)
                return under_ptr;
        }
        return nullptr;
    }

    /// ===== Menu Submenu class ===== ///

    Menu::element& Menu::submenu::operator[](size_t pos) {
        if (pos >= size()) throw 1;
        return elements[pos];
    }

    Menu::submenu& Menu::submenu::push(element element) {
        int pos = al_append_menu_item(self_menu,
            element.caption.length() > 0 ? element.caption.c_str() : nullptr,
            element.id,
            static_cast<int>(element.flags),
            nullptr,
            element.sub_menu ? element.sub_menu->self_menu : nullptr);
        element.pos = pos;
        element.self_menu = self_menu;
        element.self_menu_listing = this;

        elements.push_back(element);
        return *this;
    }

    Menu::submenu& Menu::submenu::push(submenu menu_element) {
        if (!menu_element.parent) throw 1;

        return this->push(*menu_element.parent);
    }

    bool Menu::submenu::remove(size_t pos) {
        if (pos >= size()) return false;

        if (!al_remove_menu_item(self_menu, - static_cast<int>(pos)))
            throw 1;

        elements.erase(elements.begin() + pos);
        for(size_t p = pos; p < elements.size(); ++p) {
            --elements[p].pos;
        }
        return true;
    }

    size_t Menu::submenu::size() const {
        return elements.size();
    }

    std::vector<Menu::element>::iterator Menu::submenu::begin() {
        return elements.begin();
    }

    std::vector<Menu::element>::iterator Menu::submenu::end() {
        return elements.end();
    }    

    Menu::submenu::submenu(ALLEGRO_MENU* self, element* parent)
        : self_menu(self), parent(parent)
    {}
    
} // namespace Utility
} // namespace LSWE