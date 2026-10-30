#include <LSWE/game/collisionable_object.hpp>

#include <cmath>
#include <cstring>

namespace LSWE {
namespace Game {
    
    Object& Object::move(std::function<void(Graphics::Transform&)> cb) {
        if (cb) cb(m_boundaries);

        float corners[2][3] = {
            {0, 0, 0}, {1, 1, 1}
        };

        for (size_t i = 0; i < 2; ++i) {
            auto& coords = corners[i];
            m_boundaries.transform_coordinates(coords[0], coords[1], coords[2]);
        }

        m_scale_axes = {
            fabsf(corners[1][0]-corners[0][0]),
            fabsf(corners[1][1]-corners[0][1]),
            fabsf(corners[1][2]-corners[0][2])
        };

        m_center = {
            (corners[1][0] + corners[0][0]) * 0.5f,
            (corners[1][1] + corners[0][1]) * 0.5f,
            (corners[1][2] + corners[0][2]) * 0.5f
        };

        m_inverse = m_boundaries.clone();
        m_inverse.invert_3d();

        return *this;
    }

    bool Object::work_collision(DynamicEntity& entity) {
        detail::vector3d p = entity.get_position();
        detail::vector3d o = entity.get_last_position();
        detail::vector3d d = entity.get_direction();
        
        m_inverse.transform_coordinates(p.x, p.y, p.z);
        m_inverse.transform_coordinates(o.x, o.y, o.z);

        const bool would_be_col[3] = {
            o.x >= 0.0f && o.x <= 1.0f,
            o.y >= 0.0f && o.y <= 1.0f,
            o.z >= 0.0f && o.z <= 1.0f
        };


        if (m_test_collided = (p.x >= 0.0f && p.x <= 1.0f &&
            p.y >= 0.0f && p.y <= 1.0f &&
            p.z >= 0.0f && p.z <= 1.0f))
        {
            constexpr float off = 1.01f;
            const float mx = (p.x < 0.5f ? - p.x : 1.0f - p.x);
            const float my = (p.y < 0.5f ? - p.y : 1.0f - p.y);
            const float mz = (p.z < 0.5f ? - p.z : 1.0f - p.z);
            const auto& [sx, sy, sz] = m_scale_axes.pos;

            if (!would_be_col[1]) {
                entity.combine_speed({
                    0.0f,
                    - d.y,
                    0.0f
                });
                entity.combine_position({
                    0.0f,
                    my * sy * off,
                    0.0f
                });
            }
            if (!would_be_col[0]) {
                entity.combine_speed({
                    -d.x,
                    0.0f,
                    0.0f
                });
                entity.combine_position({
                    mx * sx * off,
                    0.0f,
                    0.0f
                });
            }
            if (!would_be_col[2]) {
                entity.combine_speed({
                    0.0f,
                    0.0f,
                    -d.z,
                });
                entity.combine_position({
                    0.0f,
                    0.0f,
                    mz * sz * off
                });
            }

            return true;
        }
        return false;
    }


    detail::vector3d& DynamicEntity::modify_gravity() {
        return m_gravity;
    }

    detail::vector3d& DynamicEntity::modify_momentum() {
        return m_momentum;
    }

    void DynamicEntity::combine_position(detail::vector3d pos) {
        m_position.x += pos.x;
        m_position.y += pos.y;
        m_position.z += pos.z;
    }

    void DynamicEntity::combine_speed(detail::vector3d speed) {
        m_movement.x += speed.x;
        m_movement.y += speed.y;
        m_movement.z += speed.z;
    }

    void DynamicEntity::set_position(detail::vector3d pos) {
        m_position.x = pos.x;
        m_position.y = pos.y;
        m_position.z = pos.z;
    }

    void DynamicEntity::set_speed(detail::vector3d speed) {
        m_movement.x = speed.x;
        m_movement.y = speed.y;
        m_movement.z = speed.z;
    }

    void DynamicEntity::tick() {
        detail::vector3d calc{m_position};

        m_movement.x = m_movement.x * m_momentum.x + m_gravity.x;
        m_movement.y = m_movement.y * m_momentum.y + m_gravity.y;
        m_movement.z = m_movement.z * m_momentum.z + m_gravity.z;

        calc.x += m_movement.x;
        calc.y += m_movement.y;
        calc.z += m_movement.z;

        {
            const double cpu_now_time = al_get_time();
            m_time.delta = cpu_now_time - m_time.last_s;
            m_time.last_s = cpu_now_time;
        }

        m_last_position = m_position;
        m_position = calc;
    }

    double DynamicEntity::get_tick_delta() const {
        return m_time.delta;
    }

    detail::vector3d DynamicEntity::get_direction() const {
        return m_movement;
    }

    const detail::vector3d& DynamicEntity::get_position() const {
        return m_position;
    }

    const detail::vector3d& DynamicEntity::get_last_position() const {
        return m_last_position;
    }


} // namespace Game
} // namespace LSWE