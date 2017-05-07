#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

// Represents 4-dimensional float vector
class Vector4f
{
	float x, y, z, w;
public:
	Vector4f (float x, float y, float z, float w) : x (x), y (y), z (z), w (w) { };
	Vector4f (float x, float y, float z) : x (x), y (y), z (z), w (1.0f) { };
	Vector4f (const Vector4f& v) : x (v.x), y (v.y), z (v.z), w (v.w) { };
	Vector4f () : x (0.0f), y (0.0f), z (0.0f), w (1.0f) { };
	~Vector4f () { };
};

#endif

