#include <LSWE/graphics/transform.hpp>

#include <LSWE/utility/startup.hpp>
#include <LSWE/exception/general_null_exception.hpp>

namespace LSWE {
namespace Graphics {

    Transform::Transform() {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        m_transform = std::make_shared<ALLEGRO_TRANSFORM>();
        al_identity_transform(m_transform.get());
    }

    Transform::Transform(const ALLEGRO_TRANSFORM* copy_from) {
        Utility::SingletonOf<Utility::AllegroInit>::instance().setup();

        m_transform = std::make_shared<ALLEGRO_TRANSFORM>();
        al_copy_transform(m_transform.get(), copy_from);
    }

    Transform Transform::clone() const {
        Transform copy;
        al_copy_transform(copy.m_transform.get(), m_transform.get());
        return copy;
    }

    void Transform::copy(const ALLEGRO_TRANSFORM* copy_from) {
        if (!copy_from) throw Exception::NullException("Copying from null is not allowed");
        al_copy_transform(m_transform.get(), copy_from);
    }

    const ALLEGRO_TRANSFORM* Transform::get_current() {
        return al_get_current_transform();
    }

    const ALLEGRO_TRANSFORM* Transform::get_current_projection() {
        return al_get_current_projection_transform();
    }

    void Transform::use() const {
        al_use_transform(m_transform.get());
    }

    void Transform::use_projection() const {
        al_use_projection_transform(m_transform.get());
    }

    Transform& Transform::invert() {
        al_invert_transform(m_transform.get());
        return *this;
    }

    Transform& Transform::invert_3d() {
        al_invert_transform_3d(m_transform.get());
        return *this;
    }

    Transform& Transform::transpose() {
        al_transpose_transform(m_transform.get());
        return *this;
    }

    Transform& Transform::set_identity() {
        al_identity_transform(m_transform.get());
        return *this;
    }

    Transform& Transform::build(float x, float y, float sx, float sy, float theta) {
        al_build_transform(m_transform.get(), x, y, sx, sy, theta);
        return *this;
    }

    Transform& Transform::build_camera(float position_x, float position_y, float position_z, float look_x, float look_y, float look_z, float up_x, float up_y, float up_z) {
        al_build_camera_transform(m_transform.get(), position_x, position_y, position_z, look_x, look_y, look_z, up_x, up_y, up_z);
        return *this;
    }

    Transform& Transform::orthographic(float left, float top, float near, float right, float bottom, float far) {
        al_orthographic_transform(m_transform.get(), left, top, near, right, bottom, far);
        return *this;
    }

    Transform& Transform::perspective(float left, float top, float near, float right, float bottom, float far) {
        al_perspective_transform(m_transform.get(), left, top, near, right, bottom, far);
        return *this;
    }

    Transform& Transform::translate(float x, float y) {
        al_translate_transform(m_transform.get(), x, y);
        return *this;
    }

    Transform& Transform::translate(float x, float y, float z) {
        al_translate_transform_3d(m_transform.get(), x, y, z);
        return *this;
    }

    Transform& Transform::rotate(float theta) {
        al_rotate_transform(m_transform.get(), theta);
        return *this;
    }

    Transform& Transform::rotate(float x, float y, float z, float theta) {
        al_rotate_transform_3d(m_transform.get(), x, y, z, theta);
        return *this;
    }

    Transform& Transform::scale(float sx, float sy) {
        al_scale_transform(m_transform.get(), sx, sy);
        return *this;
    }

    Transform& Transform::scale(float sx, float sy, float sz) {
        al_scale_transform_3d(m_transform.get(), sx, sy, sz);
        return *this;
    }

    Transform& Transform::horizontal_shear(float theta) {
        al_horizontal_shear_transform(m_transform.get(), theta);
        return *this;
    }

    Transform& Transform::vertical_shear(float theta) {
        al_vertical_shear_transform(m_transform.get(), theta);
        return *this;
    }

    Transform& Transform::compose(ALLEGRO_TRANSFORM* transform) {
        al_compose_transform(m_transform.get(), transform);
        return *this;
    }

    int Transform::check_inverse(float tol) const {
        return al_check_inverse(m_transform.get(), tol);
    }

    void Transform::transform_coordinates(float& x, float& y) const {
        al_transform_coordinates(m_transform.get(), &x, &y);
    }

    void Transform::transform_coordinates(float& x, float& y, float& z) const {
        al_transform_coordinates_3d(m_transform.get(), &x, &y, &z);
    }

    void Transform::transform_coordinates(float& x, float& y, float& z, float& w) const {
        al_transform_coordinates_4d(m_transform.get(), &x, &y, &z, &w);
    }

    void Transform::transform_coordinates_projective(float& x, float& y, float& z) const {
        al_transform_coordinates_3d_projective(m_transform.get(), &x, &y, &z);
    }

    Transform::operator ALLEGRO_TRANSFORM*() const {
        return m_transform.get();
    }

} // namespace Graphics
} // namespace LSWE