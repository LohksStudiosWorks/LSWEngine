#pragma once

#include <allegro5/allegro5.h>

#include <memory>

namespace LSWE {
namespace Graphics {

    class Transform {
    public:
        Transform();
        Transform(const ALLEGRO_TRANSFORM* copy_from);

        Transform clone() const;
        void copy(const ALLEGRO_TRANSFORM* copy_from);

        static const ALLEGRO_TRANSFORM* get_current();
        static const ALLEGRO_TRANSFORM* get_current_projection();

        // more like positional transform of objects
        void use() const;
        // more for camera-like transformations
        void use_projection() const;

        Transform& invert();
        Transform& invert_3d();
        Transform& transpose();
        Transform& set_identity();
        Transform& build(float x, float y, float sx, float sy, float theta);
        Transform& build_camera(float position_x, float position_y, float position_z, float look_x, float look_y, float look_z, float up_x, float up_y, float up_z);
        Transform& orthographic(float left, float top, float near, float right, float bottom, float far);
        Transform& perspective(float left, float top, float near, float right, float bottom, float far);
        Transform& translate(float x, float y);
        Transform& translate(float x, float y, float z);
        Transform& rotate(float theta);
        Transform& rotate(float x, float y, float z, float theta);
        Transform& scale(float sx, float sy);
        Transform& scale(float sx, float sy, float sz);
        Transform& horizontal_shear(float theta);
        Transform& vertical_shear(float theta);
        Transform& compose(ALLEGRO_TRANSFORM* transform);

        int check_inverse(float tol) const;
        int check_inverse_3d(float tol) const;

        void transform_coordinates(float& x, float& y) const;
        void transform_coordinates(float& x, float& y, float& z) const;
        void transform_coordinates(float& x, float& y, float& z, float& w) const;
        void transform_coordinates_projective(float& x, float& y, float& z) const;

        operator ALLEGRO_TRANSFORM*() const;
    private:
        std::shared_ptr<ALLEGRO_TRANSFORM> m_transform;
    };
    
} // namespace Graphics
} // namespace LSWE