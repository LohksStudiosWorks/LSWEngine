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
        vector3d& vector3d::operator+=(const vector3d& oth) {
            x += oth.x;
            y += oth.y;
            z += oth.z;
            return *this;
        }
        vector3d& vector3d::operator-=(const vector3d& oth) {
            x -= oth.x;
            y -= oth.y;
            z -= oth.z;
            return *this;
        }
        vector3d& vector3d::operator*=(const vector3d& oth) {
            x *= oth.x;
            y *= oth.y;
            z *= oth.z;
            return *this;
        }
        vector3d& vector3d::operator/=(const vector3d& oth) {
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

        m_cache.m_scale_axes = {
            fabsf(corners[1][0]-corners[0][0]),
            fabsf(corners[1][1]-corners[0][1]),
            fabsf(corners[1][2]-corners[0][2])
        };

        m_cache.m_center = {
            (corners[1][0] + corners[0][0]) * 0.5f,
            (corners[1][1] + corners[0][1]) * 0.5f,
            (corners[1][2] + corners[0][2]) * 0.5f
        };

        m_cache.m_inverse = m_boundaries.clone();
        m_cache.m_inverse.invert_3d();

        m_cache.m_transformed_zero = {0.0f,0.0f,0.0f};
        
        m_cache.m_inverse.transform_coordinates(
            m_cache.m_transformed_zero.x,
            m_cache.m_transformed_zero.y,
            m_cache.m_transformed_zero.z
        );

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

    Object::collision_res Object::calculate_collision_for(detail::vector3d dir, detail::vector3d pos) {
        static Object::collision_res s_no_col = {
            .delta_pos = {0.0f,0.0f,0.0f},
            .delta_speed = {0.0f,0.0f,0.0f},
            .collided = false
        };

        detail::vector3d& p = pos;
        detail::vector3d o = pos - dir;
        bool had_non_zero = false;

        m_cache.m_inverse.transform_coordinates(p.x, p.y, p.z);
        m_cache.m_inverse.transform_coordinates(o.x, o.y, o.z);

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

            detail::vector3d d_local = transform_vector_to_local(dir);
            detail::vector3d position_corr_local{0.0f, 0.0f, 0.0f};
            detail::vector3d speed_corr_local{0.0f, 0.0f, 0.0f};

            const auto apply_slide_response = [&position_corr_local,&d_local,&speed_corr_local](const detail::vector3d& n_local, float local_offset) {
                if (n_local.x != 0.0f) position_corr_local.x += local_offset;
                if (n_local.y != 0.0f) position_corr_local.y += local_offset;
                if (n_local.z != 0.0f) position_corr_local.z += local_offset;

                float dot = d_local.x * n_local.x + d_local.y * n_local.y + d_local.z * n_local.z;
                if (dot < 0.0f) {
                    speed_corr_local.x -= dot * n_local.x;
                    speed_corr_local.y -= dot * n_local.y;
                    speed_corr_local.z -= dot * n_local.z;
                }
            };

            if (would_be_col[0] && would_be_col[1] && would_be_col[2]) {
                const float abs_mx = fabsf(mx);
                const float abs_my = fabsf(my);
                const float abs_mz = fabsf(mz);
                
                if (abs_mx < abs_my && abs_mx < abs_mz) {
                    detail::vector3d n_local = (p.x < 0.5f) ? detail::vector3d{-1.0f, 0.0f, 0.0f} : detail::vector3d{1.0f, 0.0f, 0.0f};
                    apply_slide_response(n_local, mx * off);
                    had_non_zero |= mx != 0.0f;
                }
                else if (abs_my < abs_mz) {
                    detail::vector3d n_local = (p.y < 0.5f) ? detail::vector3d{0.0f, -1.0f, 0.0f} : detail::vector3d{0.0f, 1.0f, 0.0f};
                    apply_slide_response(n_local, my * off);
                    had_non_zero |= my != 0.0f;
                }
                else {
                    detail::vector3d n_local = (p.z < 0.5f) ? detail::vector3d{0.0f, 0.0f, -1.0f} : detail::vector3d{0.0f, 0.0f, 1.0f};
                    apply_slide_response(n_local, mz * off);
                    had_non_zero |= mz != 0.0f;
                }
            }
            else {
                if (!would_be_col[0]) {
                    detail::vector3d n_local = (p.x < 0.5f) ? detail::vector3d{-1.0f, 0.0f, 0.0f} : detail::vector3d{1.0f, 0.0f, 0.0f};
                    apply_slide_response(n_local, mx * off);
                    had_non_zero |= mx != 0.0f;
                }
                if (!would_be_col[1]) {
                    detail::vector3d n_local = (p.y < 0.5f) ? detail::vector3d{0.0f, -1.0f, 0.0f} : detail::vector3d{0.0f, 1.0f, 0.0f};
                    apply_slide_response(n_local, my * off);
                    had_non_zero |= my != 0.0f;
                }
                if (!would_be_col[2]) {
                    detail::vector3d n_local = (p.z < 0.5f) ? detail::vector3d{0.0f, 0.0f, -1.0f} : detail::vector3d{0.0f, 0.0f, 1.0f};
                    apply_slide_response(n_local, mz * off);
                    had_non_zero |= mz != 0.0f;
                }
            }

            detail::vector3d world_pos_corr = transform_vector_to_world(position_corr_local);
            detail::vector3d world_speed_corr = transform_vector_to_world(speed_corr_local);
            
            return {
                .delta_pos = world_pos_corr,
                .delta_speed = world_speed_corr,
                .collided = had_non_zero
            };
        }

        return s_no_col;
    }

    Object::collision_res Object::calculate_collision_for(DynamicPoint& entity) {
        return calculate_collision_for(entity.get_direction(), entity.get_position());
    }

    Object::collision_res Object::calculate_collision_for(DynamicObject& entity) {
        static constexpr detail::vector3d local_corners[8] = {
            {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}
        };

        const float static_radius = 0.5f * std::sqrt(m_cache.m_scale_axes.x * m_cache.m_scale_axes.x + 
                                                     m_cache.m_scale_axes.y * m_cache.m_scale_axes.y + 
                                                     m_cache.m_scale_axes.z * m_cache.m_scale_axes.z);

        constexpr float dynamic_radius = 0.866f;

        const float dx = m_cache.m_center.x - entity.m_cache.m_center.x;
        const float dy = m_cache.m_center.y - entity.m_cache.m_center.y;
        const float dz = m_cache.m_center.z - entity.m_cache.m_center.z;
        const float dist_sq = dx * dx + dy * dy + dz * dz;

        const float max_allowed_dist = (static_radius + dynamic_radius) * 1.5f;
        if (dist_sq > (max_allowed_dist * max_allowed_dist)) {
            return { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, false };
        }
        
        Object::collision_res res{};


        for (const auto& local_corner : local_corners) {
            detail::vector3d world_pt_current = local_corner;
            detail::vector3d world_pt_old = local_corner;

            entity.m_boundaries.transform_coordinates(world_pt_current.x, world_pt_current.y, world_pt_current.z);
            entity.m_last_boundaries.transform_coordinates(world_pt_old.x, world_pt_old.y, world_pt_old.z);

            detail::vector3d corner_dir = world_pt_current - world_pt_old;

            if (res = calculate_collision_for(corner_dir, world_pt_current); res.collided) {
                return res;
            }
        }

        
        detail::vector3d local_center = entity.m_cache.m_center;
        m_cache.m_inverse.transform_coordinates(local_center.x, local_center.y, local_center.z);

        if (local_center.x >= 0.0f && local_center.x <= 1.0f &&
            local_center.y >= 0.0f && local_center.y <= 1.0f &&
            local_center.z >= 0.0f && local_center.z <= 1.0f) 
        {            
            const float mx = (local_center.x < 0.5f ? -local_center.x : 1.0f - local_center.x);
            const float my = (local_center.y < 0.5f ? -local_center.y : 1.0f - local_center.y);
            const float mz = (local_center.z < 0.5f ? -local_center.z : 1.0f - local_center.z);

            detail::vector3d pos_corr_local{0.0f, 0.0f, 0.0f};
            constexpr float half_size = 0.5f;
            constexpr float off = 1.05f;

            if (fabsf(mx) < fabsf(my) && fabsf(mx) < fabsf(mz)) {
                const float penetration = (local_center.x < 0.5f) ? (local_center.x + half_size) : (1.0f + half_size - local_center.x);
                pos_corr_local.x = (local_center.x < 0.5f ? -penetration : penetration) * off;
            } 
            else if (fabsf(my) < fabsf(mz)) {
                const float penetration = (local_center.y < 0.5f) ? (local_center.y + half_size) : (1.0f + half_size - local_center.y);
                pos_corr_local.y = (local_center.y < 0.5f ? -penetration : penetration) * off;
            } 
            else {
                const float penetration = (local_center.z < 0.5f) ? (local_center.z + half_size) : (1.0f + half_size - local_center.z);
                pos_corr_local.z = (local_center.z < 0.5f ? -penetration : penetration) * off;
            }
            
            res.delta_pos = transform_vector_to_world(pos_corr_local);
            res.delta_speed = {0.0f,0.0f,0.0f};
            res.collided = true;
        }

        return res;
    }

    detail::vector3d Object::transform_vector_to_world(detail::vector3d s) const {
        detail::vector3d z{0.0f,0.0f,0.0f};
        
        m_boundaries.transform_coordinates(s.x, s.y, s.z);
        m_boundaries.transform_coordinates(z.x, z.y, z.z);
        
        return s - z;
    }

    detail::vector3d Object::transform_vector_to_local(detail::vector3d s) const {        
        m_cache.m_inverse.transform_coordinates(s.x, s.y, s.z);
        
        return s - m_cache.m_transformed_zero;
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
        m_cache.m_inverse.copy(m_boundaries);
        m_cache.m_inverse.invert_3d();

        m_cache.m_transformed_zero = {0.0f,0.0f,0.0f};
        
        m_cache.m_inverse.transform_coordinates(
            m_cache.m_transformed_zero.x,
            m_cache.m_transformed_zero.y,
            m_cache.m_transformed_zero.z
        );

        m_cache.m_center += pos;
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
        const double alpha = std::min(((cpu_now_time - m_time.last_s) / std::max(m_time.delta, 1e-9)), 1.0);

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

        m_cache.m_center = {
            (corners[1][0] + corners[0][0]) * 0.5f,
            (corners[1][1] + corners[0][1]) * 0.5f,
            (corners[1][2] + corners[0][2]) * 0.5f
        };
    }

} // namespace Game
} // namespace LSWE