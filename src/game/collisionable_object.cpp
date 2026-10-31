#include <LSWE/game/collisionable_object.hpp>

#include <cmath>

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

    bool Object::work_collision(DynamicPoint& entity) {
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
            //const auto& [sx, sy, sz] = m_scale_axes.pos;

            detail::vector3d d_local = transform_vector_to_local(d);
            detail::vector3d position_corr_local{0.0f, 0.0f, 0.0f};
            detail::vector3d speed_corr_local{0.0f, 0.0f, 0.0f};

            // Helper lambda: applies the signed local offset directly and projects out inward velocity
            const auto apply_slide_response = [&](const detail::vector3d& n_local, float local_offset) {
                if (n_local.x != 0.0f) position_corr_local.x = local_offset;
                if (n_local.y != 0.0f) position_corr_local.y = local_offset;
                if (n_local.z != 0.0f) position_corr_local.z = local_offset;

                // Project velocity against surface normal to allow clean sliding
                float dot = d_local.x * n_local.x + d_local.y * n_local.y + d_local.z * n_local.z;
                if (dot < 0.0f) {
                    speed_corr_local.x -= dot * n_local.x;
                    speed_corr_local.y -= dot * n_local.y;
                    speed_corr_local.z -= dot * n_local.z;
                }
            };

            if (would_be_col[0] && would_be_col[1] && would_be_col[2]) {
                if (fabsf(d_local.x) > fabsf(d_local.y) && fabsf(d_local.x) > fabsf(d_local.z)) {
                    detail::vector3d n_local = (p.x < 0.5f) ? detail::vector3d{-1.0f, 0.0f, 0.0f} : detail::vector3d{1.0f, 0.0f, 0.0f};
                    apply_slide_response(n_local, mx * off);
                }
                else if (fabsf(d_local.y) > fabsf(d_local.z)) {
                    detail::vector3d n_local = (p.y < 0.5f) ? detail::vector3d{0.0f, -1.0f, 0.0f} : detail::vector3d{0.0f, 1.0f, 0.0f};
                    apply_slide_response(n_local, my * off);
                }
                else {
                    detail::vector3d n_local = (p.z < 0.5f) ? detail::vector3d{0.0f, 0.0f, -1.0f} : detail::vector3d{0.0f, 0.0f, 1.0f};
                    apply_slide_response(n_local, mz * off);
                }
            }
            else {
                if (!would_be_col[1]) {
                    detail::vector3d n_local = (p.y < 0.5f) ? detail::vector3d{0.0f, -1.0f, 0.0f} : detail::vector3d{0.0f, 1.0f, 0.0f};
                    apply_slide_response(n_local, my * off);
                }
                if (!would_be_col[0]) {
                    detail::vector3d n_local = (p.x < 0.5f) ? detail::vector3d{-1.0f, 0.0f, 0.0f} : detail::vector3d{1.0f, 0.0f, 0.0f};
                    apply_slide_response(n_local, mx * off);
                }
                if (!would_be_col[2]) {
                    detail::vector3d n_local = (p.z < 0.5f) ? detail::vector3d{0.0f, 0.0f, -1.0f} : detail::vector3d{0.0f, 0.0f, 1.0f};
                    apply_slide_response(n_local, mz * off);
                }
            }

            // Convert final corrections back to world space
            detail::vector3d world_pos_corr = transform_vector_to_world(position_corr_local);
            detail::vector3d world_speed_corr = transform_vector_to_world(speed_corr_local);

            entity.combine_position(world_pos_corr);
            entity.combine_speed(world_speed_corr);

            return true;
        }
        return false;
    }

    bool Object::work_collision(DynamicObject& entity) {
        detail::vector3d p = entity.get_position();
        detail::vector3d o = entity.get_last_position();
        detail::vector3d d = entity.get_direction();
        
        m_inverse.transform_coordinates(p.x, p.y, p.z);
        m_inverse.transform_coordinates(o.x, o.y, o.z);

        // get corners
        // check all corners for collision
        // merge collisions as one
        // handle from there

        return false;
    }

    const Graphics::Transform Object::get_draw_boundaries() const {
        return m_boundaries;
    }

    detail::vector3d Object::transform_vector_to_world(detail::vector3d s) const {
        detail::vector3d z{0.0f,0.0f,0.0f};
        
        m_boundaries.transform_coordinates(s.x, s.y, s.z);
        m_boundaries.transform_coordinates(z.x, z.y, z.z);
        
        return { s.x - z.x, s.y - z.y, s.z - z.z };
    }

    detail::vector3d Object::transform_vector_to_local(detail::vector3d s) const {
        detail::vector3d z{0.0f,0.0f,0.0f};
        
        m_inverse.transform_coordinates(s.x, s.y, s.z);
        m_inverse.transform_coordinates(z.x, z.y, z.z);
        
        return { s.x - z.x, s.y - z.y, s.z - z.z };
    }



    detail::vector3d& DynamicInterface::modify_gravity() {
        return m_gravity;
    }

    detail::vector3d& DynamicInterface::modify_momentum() {
        return m_momentum;
    }

    void DynamicInterface::combine_position(detail::vector3d pos) {
        (void)pos;
    }

    void DynamicInterface::combine_speed(detail::vector3d speed) {
        m_movement.x += speed.x;
        m_movement.y += speed.y;
        m_movement.z += speed.z;
    }
    
    void DynamicInterface::set_speed(detail::vector3d speed) {
        m_movement.x = speed.x;
        m_movement.y = speed.y;
        m_movement.z = speed.z;
    }

    void DynamicInterface::tick() {
        m_movement.x = m_movement.x * m_momentum.x + m_gravity.x;
        m_movement.y = m_movement.y * m_momentum.y + m_gravity.y;
        m_movement.z = m_movement.z * m_momentum.z + m_gravity.z;

        merge_movement_on_tick(); // also, do old = this, this += ... here
        
        const double cpu_now_time = al_get_time();
        m_time.delta = cpu_now_time - m_time.last_s;
        m_time.last_s = cpu_now_time;
    }

    double DynamicInterface::get_tps() const {
        return 1.0 / std::max(m_time.delta, 1e-9);
    }

    detail::vector3d DynamicInterface::get_direction() const {
        return m_movement;
    }

    void DynamicPoint::combine_position(detail::vector3d pos) {
        m_position.x += pos.x;
        m_position.y += pos.y;
        m_position.z += pos.z;
    }

    void DynamicPoint::set_position(detail::vector3d pos) {
        m_position.x = pos.x;
        m_position.y = pos.y;
        m_position.z = pos.z;
    }

    const detail::vector3d& DynamicPoint::get_position() const {
        return m_position;
    }

    const detail::vector3d& DynamicPoint::get_last_position() const {
        return m_last_position;
    }

    void DynamicPoint::merge_movement_on_tick() {
        m_last_position = m_position;
        combine_position(m_movement);
    }

    void DynamicObject::combine_position(detail::vector3d pos) {
        m_boundaries.translate(
            pos.x,
            pos.y,
            pos.z
        );
        m_inverse.translate(
            -pos.x,
            -pos.y,
            -pos.z
        );
    }

    detail::vector3d DynamicObject::get_position() const {
        detail::vector3d origin{0.0f,0.0f,0.0f};
        m_boundaries.transform_coordinates(origin.x, origin.y, origin.z);
        return origin;
    }

    detail::vector3d DynamicObject::get_last_position() const {
        detail::vector3d origin{0.0f,0.0f,0.0f};
        m_last_boundaries.transform_coordinates(origin.x, origin.y, origin.z);
        return origin;
    }

    const Graphics::Transform DynamicObject::get_draw_boundaries() const {        
        const double cpu_now_time = al_get_time();
        const double alpha = std::min(((cpu_now_time - m_time.last_s) / m_time.delta), 1.0);

        return m_last_boundaries.interpolate_with(m_boundaries, alpha);
    }
    
    void DynamicObject::merge_movement_on_tick() {
        m_last_boundaries.copy(m_boundaries);
        combine_position(m_movement);

        float corners[2][3] = {
            {0, 0, 0}, {1, 1, 1}
        };

        for (size_t i = 0; i < 2; ++i) {
            auto& coords = corners[i];
            m_boundaries.transform_coordinates(coords[0], coords[1], coords[2]);
        }

        m_center = {
            (corners[1][0] + corners[0][0]) * 0.5f,
            (corners[1][1] + corners[0][1]) * 0.5f,
            (corners[1][2] + corners[0][2]) * 0.5f
        };
    }


} // namespace Game
} // namespace LSWE