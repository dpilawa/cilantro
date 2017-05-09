#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

class Matrix4f
{
	float m[4][4];
public:
	Matrix4f () { };
	~Matrix4f () { };
	Matrix4f& InitIdentity ();
	Matrix4f& operator*= (const Matrix4f& m);
};

inline Matrix4f operator*(Matrix4f m, Matrix4f& n)
{
	m *= n;
	return m;
}

#endif