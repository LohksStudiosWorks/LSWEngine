#pragma once

#include <tuple>
#include <functional>

#include <LSWE/graphics/vertex.hpp>
#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/color.hpp>
#include <LSWE/game/player_camera.hpp>

namespace LSWE {
namespace Game {

    /* Notes for future me:
     * - Object: real world collision-able thing. It exists. It collides by its own definition.
     * - Entity: abstraction to combine with other factors and create an Object.
     */

    // from Drawable to make things drawable
    class DrawableStaticObject;


    class DynamicEntity;
    class DynamicObject;

    namespace detail {

        struct vector3d {
            union {
                float pos[3];
                struct {
                    float x, y, z;
                };
            };
        };

        struct vector2d {
            union {
                float pos[2]{};
                struct {
                    float x, y;
                };
            };
        };

        struct timing {
            double delta{};
            double last_s{};
        };

        enum class flags_movement : uint8_t {
            HAS_FREE_MOVEMENT = 0,
            HAS_COLLIDED_XP = 1 << 0,
            HAS_COLLIDED_XN = 1 << 1,
            HAS_COLLIDED_YP = 1 << 2,
            HAS_COLLIDED_YN = 1 << 3,
            HAS_COLLIDED_ZP = 1 << 4,
            HAS_COLLIDED_ZN = 1 << 5
        };
    }

    /**
     * @brief Object is a cube-like thing in the world that has collision and
     * doesn't move with interactions. It can move by itself, but it does not
     * react to external forces. Just an object somewhere there.
     */
    class Object {
    public:
        Object() = default;

        Object& move(std::function<void(Graphics::Transform&)> cb);

        bool work_collision(PlayerCamera& player);
        bool work_collision(DynamicEntity& entity);
        //bool work_collision(DynamicObject& another_object);
    protected:
        friend class DrawableStaticObject;

        Graphics::Transform m_boundaries, m_inverse;
        detail::vector3d m_scale_axes{1.0f, 1.0f, 1.0f};
        detail::vector3d m_center{};

        bool m_test_collided = false;
    };

    class DynamicEntity {
    public:
        detail::vector3d& modify_gravity();
        detail::vector3d& modify_momentum();

        void combine_position(detail::vector3d pos);
        void combine_speed(detail::vector3d speed);
        
        void tick();

        detail::vector3d get_direction() const;
        const detail::vector3d& get_position() const;
        const detail::vector3d& get_last_position() const;
    protected:
        detail::vector3d m_position{}, m_last_position{};
        detail::vector3d 
            m_gravity{}, // constant acceleration
            m_momentum{}, // constant friction
            m_movement{}; // variable movement acceleration
        detail::timing m_time;
    };

    class DynamicObject : public Object, public DynamicEntity {
    public:
        bool work_collision(Game::PlayerCamera& player);
    };



} // namespace Game

constexpr Game::detail::flags_movement operator|(
    const Game::detail::flags_movement& a,
    const Game::detail::flags_movement& b
) {
    return static_cast<Game::detail::flags_movement>(
        static_cast<std::underlying_type_t<Game::detail::flags_movement>>(a)
        | static_cast<std::underlying_type_t<Game::detail::flags_movement>>(b)
    );
}
constexpr Game::detail::flags_movement operator&(
    const Game::detail::flags_movement& a,
    const Game::detail::flags_movement& b
) {
    return static_cast<Game::detail::flags_movement>(
        static_cast<std::underlying_type_t<Game::detail::flags_movement>>(a)
        & static_cast<std::underlying_type_t<Game::detail::flags_movement>>(b)
    );
}

} // namespace LSWE