//
// Created by Matq on 03/11/2025.
//

#pragma once

#include <shared/math/Vec3.h>
#include <algorithm>
#include <cmath>

struct Quaternion
{
	float w, x, y, z;

	// Constructors
	Quaternion() : w(1), x(0), y(0), z(0) { }
	Quaternion(float W, float X, float Y, float Z) : w(W), x(X), y(Y), z(Z) { }

	// Identity quaternion (no rotation)
	static Quaternion Identity() { return { 1, 0, 0, 0 }; }

	// Create from Euler angles (pitch, yaw, roll in radians)
	// Pitch = rotation around X axis (looking up/down)
	// Yaw = rotation around Y axis (looking left/right)
	// Roll = rotation around Z axis (tilting head)
	static Quaternion FromEuler(float pitch, float yaw, float roll)
	{
		float cy = cosf(yaw * 0.5f);
		float sy = sinf(yaw * 0.5f);
		float cp = cosf(pitch * 0.5f);
		float sp = sinf(pitch * 0.5f);
		float cr = cosf(roll * 0.5f);
		float sr = sinf(roll * 0.5f);

		Quaternion q;
		q.w = cr * cp * cy + sr * sp * sy;
		q.x = sr * cp * cy - cr * sp * sy;
		q.y = cr * sp * cy + sr * cp * sy;
		q.z = cr * cp * sy - sr * sp * cy;
		return q;
	}

	// Create from axis-angle representation
	// axis: normalized direction vector
	// angle: rotation in radians
	static Quaternion FromAxisAngle(const Vec3f& axis, float angle)
	{
		float half_angle = angle * 0.5f;
		float s = sinf(half_angle);
		return Quaternion(cosf(half_angle), axis.x * s, axis.y * s, axis.z * s);
	}

	// Convert back to Euler angles (pitch, yaw, roll in radians)
	[[nodiscard]] Vec3f ToEuler() const
	{
		Vec3f euler;

		// Pitch (X-axis rotation)
		float sinp = 2.0f * (w * x + y * z);
		float cosp = 1.0f - 2.0f * (x * x + y * y);
		euler.x = atan2f(sinp, cosp);

		// Yaw (Y-axis rotation)
		float siny = 2.0f * (w * y - z * x);
		if (fabsf(siny) >= 1.0f)
			euler.y = copysignf(M_PI / 2.0f, siny); // Use 90 degrees if out of range
		else
			euler.y = asinf(siny);

		// Roll (Z-axis rotation)
		float sinr = 2.0f * (w * z + x * y);
		float cosr = 1.0f - 2.0f * (y * y + z * z);
		euler.z = atan2f(sinr, cosr);

		return euler;
	}

	// Normalize quaternion (make length = 1)
	[[nodiscard]] Quaternion Normalize() const
	{
		float len = sqrtf(w * w + x * x + y * y + z * z);
		if (len < 0.0001f) return Identity();
		float inv = 1.0f / len;
		return { w * inv, x * inv, y * inv, z * inv };
	}

	// Quaternion multiplication (combines rotations)
	// q1 * q2 means: first apply q2, then apply q1
	Quaternion operator*(const Quaternion& q) const
	{
		return {
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		};
	}

	// Rotate a vector by this quaternion
	[[nodiscard]] Vec3f RotateVector(const Vec3f& v) const
	{
		// Using formula: v' = q * v * q^(-1)
		// Optimized version without creating intermediate quaternions
		Vec3f qv(x, y, z);
		Vec3f uv = CrossProductVec3(qv, v);
		Vec3f uuv = CrossProductVec3(qv, uv);
		uv = uv * (2.0f * w);
		uuv = uuv * 2.0f;
		return v + uv + uuv;
	}

	// Conjugate (inverse for unit quaternions)
	[[nodiscard]] Quaternion Conjugate() const
	{
		return { w, -x, -y, -z };
	}

	// Linear interpolation (use for small angles only)
	static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t)
	{
		float w = a.w + t * (b.w - a.w);
		float x = a.x + t * (b.x - a.x);
		float y = a.y + t * (b.y - a.y);
		float z = a.z + t * (b.z - a.z);
		return Quaternion(w, x, y, z).Normalize();
	}

	// Spherical linear interpolation (smooth rotation interpolation)
	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t)
	{
		float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

		// If quaternions are very close, use lerp
		if (dot > 0.9995f)
		{
			return Lerp(a, b, t);
		}

		// Clamp dot product
		dot = std::clamp(dot, -1.0f, 1.0f);
		float theta = acosf(dot) * t;

		Quaternion c = b;
		c.w -= a.w * dot;
		c.x -= a.x * dot;
		c.y -= a.y * dot;
		c.z -= a.z * dot;
		c = c.Normalize();

		float s = sinf(theta);
		float c_coef = cosf(theta);

		return {
			a.w * c_coef + c.w * s,
			a.x * c_coef + c.x * s,
			a.y * c_coef + c.y * s,
			a.z * c_coef + c.z * s
		};
	}
};