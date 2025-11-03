//
// Created by Matq on 03/11/2025.
//

#pragma once

#include <shared/geometry/Quaternion.h>
#include <shared/geometry/Matrix4x4.h>
#include <shared/math/Vec3.h>
#include <shared/math/Vec2.h>

class Camera3D
{
private:
	// Position and rotation
	Vec3f pos;
	Quaternion rotation;

	// Projection parameters
	float fov_vertical;   // In radians
	float aspect_ratio;   // width / height
	float near_plane;
	float far_plane;

	// Cached derived vectors
	Vec3f right;
	Vec3f up;
	Vec3f look;

	// Cached matrices
	Mat4x4 view_matrix;
	Mat4x4 projection_matrix;
	Mat4x4 view_projection_matrix;  // Combined for efficiency

	bool matrices_dirty;

public:
	// Constructor
	Camera3D(const Vec3f& position, float fov_degrees, float aspect,
			 float near_z = 0.1f, float far_z = 1000.0f);

	// ========== POSITION ==========

	void SetPosition(const Vec3f& position);
	void Move(const Vec3f& offset);
	void MoveRelative(float forward, float right_amount, float up_amount);

	// ========== ROTATION (QUATERNION) ==========

	void SetRotation(const Quaternion& q);
	void Rotate(float pitch, float yaw, float roll);
	void RotateAround(const Vec3f& axis, float angle);
	void SetEulerAngles(float pitch, float yaw, float roll);
	void LookAt(const Vec3f& target);

	// ========== PROJECTION PARAMETERS ==========

	void SetFOV(float degrees);
	void SetAspectRatio(float aspect);
	void UpdateAspectRatio(int screen_width, int screen_height);
	void SetClipPlanes(float near_z, float far_z);

	// ========== GETTERS ==========

	[[nodiscard]] Vec3f GetPosition() const { return pos; }
	[[nodiscard]] Vec3f GetLook() const { return look; }
	[[nodiscard]] Vec3f GetUp() const { return up; }
	[[nodiscard]] Vec3f GetRight() const { return right; }
	[[nodiscard]] Quaternion GetRotation() const { return rotation; }
	[[nodiscard]] Vec3f GetEulerAngles() const { return rotation.ToEuler(); }

	[[nodiscard]] const Mat4x4& GetViewMatrix() const;
	[[nodiscard]] const Mat4x4& GetProjectionMatrix() const;
	[[nodiscard]] const Mat4x4& GetViewProjectionMatrix() const;

	// ========== PROJECTION METHODS ==========

	Vec2f CameraToScreen(const Vec3f& world_pos, bool& visible, const Vec2f& screen_size) const;
	bool IsPointVisible(const Vec3f& world_pos) const;

private:
	void UpdateVectors();
	void UpdateMatrices();
};