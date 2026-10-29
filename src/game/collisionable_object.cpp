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

    bool Object::work_collision(PlayerCamera& player) {
        float x = player.get_x();
        float y = player.get_y();
        float z = player.get_z();
        
        m_inverse.transform_coordinates(x, y, z);

        if (m_test_collided = (x >= 0.0f && x <= 1.0f &&
            y >= 0.0f && y <= 1.0f &&
            z >= 0.0f && z <= 1.0f))
        {
            constexpr float off = 1.0001f;
            const float mx = (x < 0.5f ? - x : 1.0f - x);
            const float my = (y < 0.5f ? - y : 1.0f - y);
            const float mz = (z < 0.5f ? - z : 1.0f - z);
            const auto& [sx, sy, sz] = m_scale_axes.pos;

            if (my != 0.0f && fabsf(my) < 0.2f) {
                player.world_block_move_towards_y(my * sy * off);
            }
            else if (mx != 0.0f && fabsf(mx) * sx < fabsf(mz) * sz) {
                player.world_block_move_towards_x(mx * sx * off);
            }
            else {
                player.world_block_move_towards_z(mz * sz * off);
            }
            return true;
        }
        return false;
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
                printf("SY=%.3f\n", d.y);
                entity.combine_speed({
                    0.0f,
                    - d.y,// * off + my * sy * off,
                    0.0f
                });
                entity.combine_position({
                    0.0f,
                    my * sy * off,
                    0.0f
                });
            }
            if (!would_be_col[0]) {
                printf("SX=%.3f\n", d.x);
                entity.combine_speed({
                    -d.x,// * off + mx * sx * off,
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
                printf("SZ=%.3f\n", d.z);
                entity.combine_speed({
                    0.0f,
                    0.0f,
                    -d.z,// * off + mz * sz * off
                });
                entity.combine_position({
                    0.0f,
                    0.0f,
                    mz * sz * off
                });
            }


            //printf("%.2f %.2f %.2f\n", - d.x, - d.y, - d.z);


            /*constexpr float off = 1.0001f;
            const float mx = (p.x < 0.5f ? - p.x : 1.0f - p.x);
            const float my = (p.y < 0.5f ? - p.y : 1.0f - p.y);
            const float mz = (p.z < 0.5f ? - p.z : 1.0f - p.z);
            const auto& [sx, sy, sz] = m_scale_axes.pos;

            if (my != 0.0f && fabsf(my) < 0.2f) {
                entity.combine_speed({0.0f, my * sy * off, 0.0f});
            }
            else if (mx != 0.0f && fabsf(mx) * sx < fabsf(mz) * sz) {
                entity.combine_speed({mx * sx * off, 0.0f, 0.0f});
            }
            else {
                entity.combine_speed({0.0f, 0.0f, mz * sz * off});
            }*/
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

        printf("P: %.2f %.2f %.2f\n", calc.x, calc.y, calc.z);
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