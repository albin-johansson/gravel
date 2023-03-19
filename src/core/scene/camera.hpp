#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/ecs.hpp"
#include "common/type/math.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_S(Transform);

/// Context component for tracking the active camera.
struct CameraContext final {
  Entity active_camera {kNullEntity};
};

/// Component for a perspective camera.
/// The position and direction are delegated to the transform component.
struct Camera final {
  Vec3 up {0, 1, 0};                    ///< Normalized up direction.
  float32 fov {90};                     ///< Vertical field-of-view, in degrees.
  float32 near_plane {0.1f};            ///< Distance to frustum near plane.
  float32 far_plane {100'000};          ///< Distance to frustum far plane.
  float32 aspect_ratio {16.0f / 9.0f};  ///< Far plane aspect ratio,
};

/// Rotates a camera with the specified amount along the yaw and pitch axes.
///
/// \param camera the camera component.
/// \param transform the transform component associated with the camera.
/// \param yaw the rotation along the yaw axis, in radians.
/// \param pitch the rotation along the pitch axis, in radians.
void rotate_camera(const Camera& camera,
                   Transform& transform,
                   float32 yaw,
                   float32 pitch);

/// Creates a projection matrix based on a camera.
///
/// \param camera the camera component.
/// \return a projection matrix.
[[nodiscard]] auto to_projection_matrix(const Camera& camera) -> Mat4;

/// Creates a view matrix based on a camera.
///
/// \param camera the camera component.
/// \param transform the transform component associated with the camera.
/// \return a view matrix.
[[nodiscard]] auto to_view_matrix(const Camera& camera, const Transform& transform)
    -> Mat4;

}  // namespace gravel
