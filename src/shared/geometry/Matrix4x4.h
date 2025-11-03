//
// Created by Matq on 03/11/2025.
//

#pragma once

#include <shared/geometry/Quaternion.h>
#include <shared/math/Vec4.h>
#include <shared/math/Vec3.h>

struct Mat4x4 {
	float m[16];

	// Constructor - identity matrix by default
	Mat4x4() {
		for (float & i : m) i = 0;
		m[0] = m[5] = m[10] = m[15] = 1.0f;
	}

	// Access element at row i, column j
	[[nodiscard]] float& at(int row, int col) { return m[col * 4 + row]; }
	[[nodiscard]] const float& at(int row, int col) const { return m[col * 4 + row]; }

	// Identity matrix
	static Mat4x4 Identity() { return {}; }

	// Translation matrix
	static Mat4x4 Translation(const Vec3f& pos) {
		Mat4x4 mat;
		mat.at(0, 3) = pos.x;
		mat.at(1, 3) = pos.y;
		mat.at(2, 3) = pos.z;
		return mat;
	}

	// Scale matrix
	static Mat4x4 Scale(const Vec3f& scale) {
		Mat4x4 mat;
		mat.at(0, 0) = scale.x;
		mat.at(1, 1) = scale.y;
		mat.at(2, 2) = scale.z;
		return mat;
	}

	// Create rotation matrix from quaternion
	static Mat4x4 FromQuaternion(const Quaternion& q) {
		Mat4x4 mat;

		float xx = q.x * q.x;
		float yy = q.y * q.y;
		float zz = q.z * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;

		mat.at(0, 0) = 1.0f - 2.0f * (yy + zz);
		mat.at(0, 1) = 2.0f * (xy - wz);
		mat.at(0, 2) = 2.0f * (xz + wy);

		mat.at(1, 0) = 2.0f * (xy + wz);
		mat.at(1, 1) = 1.0f - 2.0f * (xx + zz);
		mat.at(1, 2) = 2.0f * (yz - wx);

		mat.at(2, 0) = 2.0f * (xz - wy);
		mat.at(2, 1) = 2.0f * (yz + wx);
		mat.at(2, 2) = 1.0f - 2.0f * (xx + yy);

		return mat;
	}

	static Mat4x4 LookAt(const Vec3f& eye, const Vec3f& target, const Vec3f& up) {
		Vec3f f = (target - eye).NormalizeF();  // Forward
		Vec3f r = CrossProductVec3(f, up).NormalizeF();  // Right
		Vec3f u = CrossProductVec3(r, f);  // Up

		Mat4x4 mat;
		mat.at(0, 0) = -r.x;  // Negated
		mat.at(0, 1) = -r.y;  // Negated
		mat.at(0, 2) = -r.z;  // Negated
		mat.at(0, 3) = DotProductVec3(r, eye);  // Negated

		mat.at(1, 0) = u.x;
		mat.at(1, 1) = u.y;
		mat.at(1, 2) = u.z;
		mat.at(1, 3) = -DotProductVec3(u, eye);

		mat.at(2, 0) = -f.x;
		mat.at(2, 1) = -f.y;
		mat.at(2, 2) = -f.z;
		mat.at(2, 3) = DotProductVec3(f, eye);

		mat.at(3, 0) = 0;
		mat.at(3, 1) = 0;
		mat.at(3, 2) = 0;
		mat.at(3, 3) = 1;

		return mat;
	}

	// Perspective projection matrix
	static Mat4x4 Perspective(float fov_radians, float aspect, float near_z, float far_z) {
		float tan_half_fov = tanf(fov_radians / 2.0f);

		Mat4x4 mat;
		mat.at(0, 0) = 1.0f / (aspect * tan_half_fov);
		mat.at(1, 1) = 1.0f / tan_half_fov;
		mat.at(2, 2) = -(far_z + near_z) / (far_z - near_z);
		mat.at(2, 3) = -(2.0f * far_z * near_z) / (far_z - near_z);
		mat.at(3, 2) = -1.0f;
		mat.at(3, 3) = 0.0f;

		return mat;
	}

	// Matrix multiplication
	Mat4x4 operator*(const Mat4x4& other) const {
		Mat4x4 result;
		for (int row = 0; row < 4; row++) {
			for (int col = 0; col < 4; col++) {
				result.at(row, col) =
					at(row, 0) * other.at(0, col) +
						at(row, 1) * other.at(1, col) +
						at(row, 2) * other.at(2, col) +
						at(row, 3) * other.at(3, col);
			}
		}
		return result;
	}

	// Transform Vec3f (assumes w=1, returns xyz)
	[[nodiscard]] Vec3f TransformPoint(const Vec3f& v) const {
		float x = at(0,0) * v.x + at(0,1) * v.y + at(0,2) * v.z + at(0,3);
		float y = at(1,0) * v.x + at(1,1) * v.y + at(1,2) * v.z + at(1,3);
		float z = at(2,0) * v.x + at(2,1) * v.y + at(2,2) * v.z + at(2,3);
		float w = at(3,0) * v.x + at(3,1) * v.y + at(3,2) * v.z + at(3,3);

		if (w != 0.0f)
			return {x / w, y / w, z / w};

		return {x, y, z};
	}

	// Transform Vec4f
	Vec4f operator*(const Vec4f& v) const {
		return {
			at(0,0) * v.x + at(0,1) * v.y + at(0,2) * v.z + at(0,3) * v.w,
			at(1,0) * v.x + at(1,1) * v.y + at(1,2) * v.z + at(1,3) * v.w,
			at(2,0) * v.x + at(2,1) * v.y + at(2,2) * v.z + at(2,3) * v.w,
			at(3,0) * v.x + at(3,1) * v.y + at(3,2) * v.z + at(3,3) * v.w
		};
	}
};