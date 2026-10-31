#include <LSWE/game/collisionable_object.hpp>

#include <cmath>

namespace LSWE {
namespace Game {

    namespace detail {

        vector3d& vector3d::operator+=(float factor) {
            x += factor;
            y += factor;
            z += factor;
            return *this;
        }
        vector3d& vector3d::operator-=(float factor) {
            x -= factor;
            y -= factor;
            z -= factor;
            return *this;
        }
        vector3d& vector3d::operator*=(float factor) {
            x *= factor;
            y *= factor;
            z *= factor;
            return *this;
        }
        vector3d& vector3d::operator/=(float factor) {
            x /= factor;
            y /= factor;
            z /= factor;
            return *this;
        }
        vector3d& vector3d::operator+=(vector3d oth) {
            x += oth.x;
            y += oth.y;
            z += oth.z;
            return *this;
        }
        vector3d& vector3d::operator-=(vector3d oth) {
            x -= oth.x;
            y -= oth.y;
            z -= oth.z;
            return *this;
        }
        vector3d& vector3d::operator*=(vector3d oth) {
            x *= oth.x;
            y *= oth.y;
            z *= oth.z;
            return *this;
        }
        vector3d& vector3d::operator/=(vector3d oth) {
            x /= oth.x;
            y /= oth.y;
            z /= oth.z;
            return *this;
        }
    }
    
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
        auto res = calculate_collision_for(entity);
        if (res.collided) {
            entity.combine_position(res.delta_pos);
            entity.combine_speed(res.delta_speed);
        }
        return res.collided;
    }

    bool Object::work_collision(DynamicObject& entity) {
        auto res = calculate_collision_for(entity);
        if (res.collided) {
            entity.combine_position(res.delta_pos);
            entity.combine_speed(res.delta_speed);
        }
        return res.collided;
    }

    const Graphics::Transform Object::get_draw_boundaries() const {
        return m_boundaries;
    }

    Object::collision_res Object::calculate_collision_for(DynamicPoint& entity) {
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

        if (p.x >= 0.0f && p.x <= 1.0f &&
            p.y >= 0.0f && p.y <= 1.0f &&
            p.z >= 0.0f && p.z <= 1.0f)
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

            //entity.combine_position(world_pos_corr);
            //entity.combine_speed(world_speed_corr);

            return {
                .delta_pos = world_pos_corr,
                .delta_speed = world_speed_corr,
                .collided = true
            };
        }

        return {
            .delta_pos = {0.0f,0.0f,0.0f},
            .delta_speed = {0.0f,0.0f,0.0f},
            .collided = false
        };
    }

    Object::collision_res Object::calculate_collision_for(DynamicObject& entity) {
        static constexpr detail::vector3d local_corners[8] = {
            {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}
        };

        detail::vector3d d = entity.get_direction(); // Moving object's speed/velocity
        detail::vector3d res_pos{0.0f,0.0f,0.0f};
        detail::vector3d res_speed{0.0f,0.0f,0.0f};

        bool collided = false;

        for (const auto& local_corner : local_corners) {
            // Step 1: Transform 'other's local corner into World Space using 'other.m_transform'
            detail::vector3d world_pt = local_corner;
            entity.m_boundaries.transform_coordinates(world_pt.x, world_pt.y, world_pt.z);

            // Step 2: Transform that world point into 'this' (static) object's Local Space using 'this->m_inverse'
            detail::vector3d p = world_pt;
            m_inverse.transform_coordinates(p.x, p.y, p.z);

            // Step 3: Check if the corner falls inside 'this' (static) object's [0..1] box
            if (p.x >= 0.0f && p.x <= 1.0f &&
                p.y >= 0.0f && p.y <= 1.0f &&
                p.z >= 0.0f && p.z <= 1.0f)
            {
                collided = true;
                constexpr float off = 1.01f;

                const float mx = (p.x < 0.5f ? -p.x : 1.0f - p.x);
                const float my = (p.y < 0.5f ? -p.y : 1.0f - p.y);
                const float mz = (p.z < 0.5f ? -p.z : 1.0f - p.z);

                detail::vector3d d_local = transform_vector_to_local(d);
                detail::vector3d position_corr_local{0.0f, 0.0f, 0.0f};
                detail::vector3d speed_corr_local{0.0f, 0.0f, 0.0f};

                // Lambda to apply position correction and project out inward velocity for sliding
                auto apply_slide_response = [&](const detail::vector3d& n_local, float local_offset) {
                    if (n_local.x != 0.0f) position_corr_local.x = local_offset;
                    if (n_local.y != 0.0f) position_corr_local.y = local_offset;
                    if (n_local.z != 0.0f) position_corr_local.z = local_offset;

                    float dot = d_local.x * n_local.x + d_local.y * n_local.y + d_local.z * n_local.z;
                    if (dot < 0.0f) {
                        speed_corr_local.x -= dot * n_local.x;
                        speed_corr_local.y -= dot * n_local.y;
                        speed_corr_local.z -= dot * n_local.z;
                    }
                };

                // Find the dominant axis of penetration relative to 'this'
                if (fabsf(mx) < fabsf(my) && fabsf(mx) < fabsf(mz)) {
                    detail::vector3d n_local = (p.x < 0.5f) ? detail::vector3d{-1.0f, 0.0f, 0.0f} : detail::vector3d{1.0f, 0.0f, 0.0f};
                    apply_slide_response(n_local, mx * off);
                }
                else if (fabsf(my) < fabsf(mz)) {
                    detail::vector3d n_local = (p.y < 0.5f) ? detail::vector3d{0.0f, -1.0f, 0.0f} : detail::vector3d{0.0f, 1.0f, 0.0f};
                    apply_slide_response(n_local, my * off);
                }
                else {
                    detail::vector3d n_local = (p.z < 0.5f) ? detail::vector3d{0.0f, 0.0f, -1.0f} : detail::vector3d{0.0f, 0.0f, 1.0f};
                    apply_slide_response(n_local, mz * off);
                }

                // Convert corrections back to world space using 'this' (static) object's transform matrix
                detail::vector3d world_pos_corr = transform_vector_to_world(position_corr_local);
                detail::vector3d world_speed_corr = transform_vector_to_world(speed_corr_local);

                // Apply corrections exclusively to the moving object ('other')
                //entity.combine_position(world_pos_corr);
                //entity.combine_speed(world_speed_corr);
                res_pos += world_pos_corr;
                res_speed += world_speed_corr;

                //entity.m_test_collided = true;
                break; // Stop after handling the first colliding corner this tick
            }
        }

        return {
            .delta_pos = res_pos,
            .delta_speed = res_speed,
            .collided = collided
        };
    }

    detail::vector3d Object::transform_vector_to_world(detail::vector3d s) const {
        detail::vector3d z{0.0f,0.0f,0.0f};
        
        m_boundaries.transform_coordinates(s.x, s.y, s.z);
        m_boundaries.transform_coordinates(z.x, z.y, z.z);
        
        return s - z;
    }

    detail::vector3d Object::transform_vector_to_local(detail::vector3d s) const {
        detail::vector3d z{0.0f,0.0f,0.0f};
        
        m_inverse.transform_coordinates(s.x, s.y, s.z);
        m_inverse.transform_coordinates(z.x, z.y, z.z);
        
        return s - z;
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
        m_movement += speed;
    }
    
    void DynamicInterface::set_speed(detail::vector3d speed) {
        m_movement = speed;
    }

    void DynamicInterface::tick() {
        m_movement = m_movement * m_momentum + m_gravity;

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
        m_position += pos;
    }

    void DynamicPoint::set_position(detail::vector3d pos) {
        m_position = pos;
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

    bool DynamicObject::work_collision(DynamicPoint& entity) {
        auto res = calculate_collision_for(entity);
        if (res.collided) {
            res.delta_pos *= 0.5f;
            res.delta_speed *= 0.5f;
            entity.combine_position(res.delta_pos);
            entity.combine_speed(res.delta_speed);
            res.delta_pos *= -1.0f;
            res.delta_speed *= -1.0f;
            combine_position(res.delta_pos);
            combine_speed(res.delta_speed);
        }
        return res.collided;
    }

    bool DynamicObject::work_collision(DynamicObject& entity) {
        auto res = calculate_collision_for(entity);
        if (res.collided) {
            res.delta_pos *= 0.5f;
            res.delta_speed *= 0.5f;
            entity.combine_position(res.delta_pos);
            entity.combine_speed(res.delta_speed);
            res.delta_pos *= -1.0f;
            res.delta_speed *= -1.0f;
            combine_position(res.delta_pos);
            combine_speed(res.delta_speed);
        }
        return res.collided;
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