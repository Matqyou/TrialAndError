//
// Created by Matq on 03/11/2025.
//

#include "Camera3D.h"
#include <shared/string/Strings.h>
#include <cmath>

// Constructor
Camera3D::Camera3D(const Vec3f& position, float fov_degrees, float aspect,
				   float near_z, float far_z)
	: pos(position),
	  rotation(Quaternion::Identity()),
	  fov_vertical(fov_degrees * M_PI / 180.0f),
	  aspect_ratio(aspect),
	  near_plane(near_z),
	  far_plane(far_z),
	  matrices_dirty(true)
{
	UpdateVectors();
	UpdateMatrices();
}

// ========== POSITION ==========

void Camera3D::SetPosition(const Vec3f& position)
{
	pos = position;
	matrices_dirty = true;
}

void Camera3D::Move(const Vec3f& offset)
{
	pos = pos + offset;
	matrices_dirty = true;
}

void Camera3D::MoveRelative(float forward, float right_amount, float up_amount)
{
	pos = pos + look * forward + right * right_amount + up * up_amount;
	matrices_dirty = true;
}

// ========== ROTATION (QUATERNION) ==========

void Camera3D::SetRotation(const Quaternion& q)
{
	rotation = q.Normalize();
	matrices_dirty = true;
	UpdateVectors();
}

void Camera3D::Rotate(float pitch, float yaw, float roll)
{
	Quaternion q = Quaternion::FromEuler(pitch, yaw, roll);
	rotation = (rotation * q).Normalize();
	matrices_dirty = true;
	UpdateVectors();
}

void Camera3D::RotateAround(const Vec3f& axis, float angle)
{
	Quaternion q = Quaternion::FromAxisAngle(axis, angle);
	rotation = (rotation * q).Normalize();
	matrices_dirty = true;
	UpdateVectors();
}

void Camera3D::SetEulerAngles(float pitch, float yaw, float roll)
{
	rotation = Quaternion::FromEuler(pitch, yaw, roll);
	matrices_dirty = true;
	UpdateVectors();
}

void Camera3D::LookAt(const Vec3f& target)
{
	Vec3f forward = (target - pos).NormalizeF();
	Vec3f right = CrossProductVec3(forward, Vec3f(0, 1, 0)).NormalizeF();
	Vec3f up = CrossProductVec3(right, forward);

	// Build rotation matrix and convert to quaternion
	Mat4x4 rot_mat = Mat4x4::Identity();
	rot_mat.at(0, 0) = right.x;
	rot_mat.at(0, 1) = right.y;
	rot_mat.at(0, 2) = right.z;
	rot_mat.at(1, 0) = up.x;
	rot_mat.at(1, 1) = up.y;
	rot_mat.at(1, 2) = up.z;
	rot_mat.at(2, 0) = -forward.x;
	rot_mat.at(2, 1) = -forward.y;
	rot_mat.at(2, 2) = -forward.z;

	// Convert matrix to quaternion (simplified)
	float trace = rot_mat.at(0, 0) + rot_mat.at(1, 1) + rot_mat.at(2, 2);
	if (trace > 0)
	{
		float s = sqrtf(trace + 1.0f) * 2.0f;
		rotation.w = 0.25f * s;
		rotation.x = (rot_mat.at(2, 1) - rot_mat.at(1, 2)) / s;
		rotation.y = (rot_mat.at(0, 2) - rot_mat.at(2, 0)) / s;
		rotation.z = (rot_mat.at(1, 0) - rot_mat.at(0, 1)) / s;
	}
	else
	{
		// Fallback for trace <= 0
		rotation = Quaternion::Identity();
	}

	matrices_dirty = true;
	UpdateVectors();
}

// ========== PROJECTION PARAMETERS ==========

void Camera3D::SetFOV(float degrees)
{
	fov_vertical = degrees * static_cast<float>(M_PI) / 180.0f;
	matrices_dirty = true;
}

void Camera3D::SetAspectRatio(float aspect)
{
	aspect_ratio = aspect;
	matrices_dirty = true;
}

void Camera3D::UpdateAspectRatio(int screen_width, int screen_height)
{
	aspect_ratio = (float)screen_width / (float)screen_height;
	matrices_dirty = true;
}

void Camera3D::SetClipPlanes(float near_z, float far_z)
{
	near_plane = near_z;
	far_plane = far_z;
	matrices_dirty = true;
}

// ========== GETTERS ==========

const Mat4x4& Camera3D::GetViewMatrix() const
{
	if (matrices_dirty) const_cast<Camera3D*>(this)->UpdateMatrices();
	return view_matrix;
}

const Mat4x4& Camera3D::GetProjectionMatrix() const
{
	if (matrices_dirty) const_cast<Camera3D*>(this)->UpdateMatrices();
	return projection_matrix;
}

const Mat4x4& Camera3D::GetViewProjectionMatrix() const
{
	if (matrices_dirty) const_cast<Camera3D*>(this)->UpdateMatrices();
	return view_projection_matrix;
}

// ========== PROJECTION METHODS ==========

Vec2f Camera3D::CameraToScreen(const Vec3f& world_pos, bool& visible, const Vec2f& screen_size) const
{
	if (matrices_dirty) const_cast<Camera3D*>(this)->UpdateMatrices();

	// Transform to clip space
	Vec4f clip = view_projection_matrix * Vec4f(world_pos.x, world_pos.y, world_pos.z, 1.0f);

	// Debug: Check for invalid values
	if (std::isnan(clip.x) || std::isnan(clip.y) || std::isnan(clip.z) || std::isnan(clip.w))
	{
		dbg_msg("NaN in clip space! clip: %.2f %.2f %.2f %.2f\n", clip.x, clip.y, clip.z, clip.w);
		visible = false;
		return { 0, 0 };
	}

	// Behind camera?
	if (clip.w <= 0.0f)
	{
		visible = false;
		return { 0, 0 };
	}

	// Perspective divide to get NDC (-1 to 1)
	Vec3f ndc(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);

	// Check if in frustum
	visible = (ndc.x >= -1.0f && ndc.x <= 1.0f &&
		ndc.y >= -1.0f && ndc.y <= 1.0f &&
		ndc.z >= -1.0f && ndc.z <= 1.0f);

	// NDC to screen space
	float screen_x = (ndc.x + 1.0f) * 0.5f * screen_size.x;
	float screen_y = (1.0f - ndc.y) * 0.5f * screen_size.y;  // Flip Y

	return { screen_x, screen_y };
}

bool Camera3D::IsPointVisible(const Vec3f& world_pos) const
{
	Vec3f to_point = world_pos - pos;
	float distance = DotProductVec3(to_point, look);

	// Check if in depth range
	if (distance < near_plane || distance > far_plane)
		return false;

	// Check if in view frustum (rough check)
	Vec3f dir = to_point.NormalizeF();
	float angle = DotProductVec3(dir, look);
	float half_fov = fov_vertical * 0.5f;

	return angle > cosf(half_fov);
}

// ========== PRIVATE METHODS ==========

void Camera3D::UpdateVectors()
{
	look = rotation.RotateVector(Vec3f(0, 0, 1));   // Forward (Z+)
	up = rotation.RotateVector(Vec3f(0, 1, 0));     // Up (Y+)
	right = rotation.RotateVector(Vec3f(1, 0, 0));  // Right (X+)
}

void Camera3D::UpdateMatrices()
{
	if (!matrices_dirty) return;

	// View matrix
	view_matrix = Mat4x4::LookAt(pos, pos + look, up);

	// Projection matrix
	projection_matrix = Mat4x4::Perspective(fov_vertical, aspect_ratio, near_plane, far_plane);

	// Combined matrix (more efficient for projection)
	view_projection_matrix = projection_matrix * view_matrix;

	matrices_dirty = false;
}