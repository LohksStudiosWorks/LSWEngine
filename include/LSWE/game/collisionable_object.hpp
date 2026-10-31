#pragma once

#include <tuple>
#include <functional>

#include <LSWE/graphics/vertex.hpp>
#include <LSWE/graphics/transform.hpp>
#include <LSWE/graphics/color.hpp>

namespace LSWE {
namespace Game {

    /* Notes for future me:
     * - Object: real world collision-able thing. It exists. It collides by its own definition.
     * - Entity: abstraction to combine with other factors and create an Object.
     */

    // from Drawable to make things drawable
    class WrapObjectDrawable;
    class Entity;


    class DynamicPoint;
    class DynamicObject;

    namespace detail {

        struct vector3d {
            union {
                float pos[3];
                struct {
                    float x, y, z;
                };
            };

            vector3d& operator+=(float factor);
            vector3d& operator-=(float factor);
            vector3d& operator*=(float factor);
            vector3d& operator/=(float factor);
            vector3d& operator+=(vector3d oth);
            vector3d& operator-=(vector3d oth);
            vector3d& operator*=(vector3d oth);
            vector3d& operator/=(vector3d oth);
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

        virtual bool work_collision(DynamicPoint& entity);
        virtual bool work_collision(DynamicObject& entity);

        virtual const Graphics::Transform get_draw_boundaries() const;
    protected:
        struct collision_res {
            detail::vector3d delta_pos;
            detail::vector3d delta_speed;
            bool collided;
        };

        detail::vector3d transform_vector_to_world(detail::vector3d s) const;
        detail::vector3d transform_vector_to_local(detail::vector3d s) const;

        collision_res calculate_collision_for(DynamicPoint& entity);
        collision_res calculate_collision_for(DynamicObject& entity);

        friend class WrapObjectDrawable;
        friend class Entity;

        Graphics::Transform m_boundaries, m_inverse;
        detail::vector3d m_scale_axes{1.0f, 1.0f, 1.0f};
        detail::vector3d m_center{};
    };

    class DynamicInterface {
    public:
        detail::vector3d& modify_gravity();
        detail::vector3d& modify_momentum();

        virtual void combine_position(detail::vector3d pos);
        virtual void combine_speed(detail::vector3d speed);

        void set_speed(detail::vector3d speed);

        void tick();
        double get_tps() const;

        detail::vector3d get_direction() const;
    protected:
        virtual void merge_movement_on_tick() = 0;

        detail::vector3d 
            m_gravity{}, // constant acceleration
            m_momentum{}, // constant friction
            m_movement{}; // variable movement acceleration
        detail::timing m_time;
    };

    class DynamicPoint : public DynamicInterface {
    public:
        void combine_position(detail::vector3d pos) override;

        void set_position(detail::vector3d pos);
        
        const detail::vector3d& get_position() const;
        const detail::vector3d& get_last_position() const;
    protected:
        void merge_movement_on_tick() override;

        detail::vector3d m_position{}, m_last_position{};
    };

    class DynamicObject : public Object, public DynamicInterface {
    public:
        void combine_position(detail::vector3d pos) override;

        bool work_collision(DynamicPoint& entity) override;
        bool work_collision(DynamicObject& entity) override;
        
        detail::vector3d get_position() const;
        detail::vector3d get_last_position() const;

        const Graphics::Transform get_draw_boundaries() const override;
    protected:
        void merge_movement_on_tick() override;

        friend class Object;
        friend class Entity;

        Graphics::Transform m_last_boundaries;
        detail::timing m_time;
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

// --- Vector & Vector Operators ---
inline Game::detail::vector3d operator+(Game::detail::vector3d lhs, const Game::detail::vector3d& rhs) { return lhs += rhs; }
inline Game::detail::vector3d operator-(Game::detail::vector3d lhs, const Game::detail::vector3d& rhs) { return lhs -= rhs; }
inline Game::detail::vector3d operator*(Game::detail::vector3d lhs, const Game::detail::vector3d& rhs) { return lhs *= rhs; }
inline Game::detail::vector3d operator/(Game::detail::vector3d lhs, const Game::detail::vector3d& rhs) { return lhs /= rhs; }

// --- Vector & Scalar Operators (Vector op Scalar) ---
inline Game::detail::vector3d operator+(Game::detail::vector3d lhs, float factor) { return lhs += factor; }
inline Game::detail::vector3d operator-(Game::detail::vector3d lhs, float factor) { return lhs -= factor; }
inline Game::detail::vector3d operator*(Game::detail::vector3d lhs, float factor) { return lhs *= factor; }
inline Game::detail::vector3d operator/(Game::detail::vector3d lhs, float factor) { return lhs /= factor; }

} // namespace LSWE