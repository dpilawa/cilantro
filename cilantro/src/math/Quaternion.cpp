#include "math/Quaternion.h"

// constructors
Quaternion::Quaternion () { };
Quaternion::Quaternion (float a, float b, float c, float d) : s (a), v {b, c, d} { };
Quaternion::Quaternion (float scalar, Vector3f vector) : s (scalar), v (vector) { };

// copy constructor
Quaternion::Quaternion (const Quaternion& other)
{
    s = other.s;
    v = other.v;
}

// move constructor
Quaternion::Quaternion (Quaternion&& other) : Quaternion ()
{
    std::swap (s, other.s);
    v = std::move (other.v);
}

// assignment operator
Quaternion& Quaternion::operator= (Quaternion other)
{
    std::swap (s, other.s);
    v = std::move (other.v);
    return *this;
}

// destructor
Quaternion::~Quaternion () { };


Quaternion& Quaternion::operator*=(float f)
{
    s *= f;
    v *= f;

    return *this;
};

Quaternion& Quaternion::operator+=(const Quaternion& other)
{
    s += other.s;
    v += other.v;

    return *this;
};

Quaternion& Quaternion::operator-=(const Quaternion& other)
{
    s -= other.s;
    v -= other.v;

    return *this;
};

Quaternion operator* (Quaternion q, float f)
{
    q *= f;
    return q;
};

Quaternion operator* (float f, Quaternion q)
{
    q *= f;
    return q;
};

Quaternion operator+ (Quaternion q, const Quaternion& r)
{
    q += r;
    return q;
};

Quaternion operator- (Quaternion q, const Quaternion& r)
{
    q -= r;
    return q;
};

Quaternion operator- (Quaternion q)
{
    q.s = -q.s;
    q.v = -q.v;

    return q;
}