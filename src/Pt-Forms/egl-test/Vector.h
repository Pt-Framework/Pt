 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
    Copyright (C) 2016 Ilja Maier
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA  02110-1301  USA
*/

#ifndef Pt_Forms_Vector_H
#define Pt_Forms_Vector_H

#include <cfloat>
#include <cmath>

namespace Pt {

namespace Forms {

// vec2
struct vec2
{
    float x, y;

    inline static float length(const vec2& u)
    {
        return sqrt(u.x*u.x + u.y*u.y);
    }

    inline static vec2 normalize(const vec2& u)
    {
        float invmag = 1.0f/(length(u) + FLT_EPSILON);
        return vec2(u.x * invmag, u.y * invmag);
    }

    inline vec2()
    {
    }

    inline vec2(float x, float y) 
      : x(x), y(y)
    {
    }

    friend vec2 operator +(const vec2& u, const vec2& v);
    friend vec2 operator -(const vec2& u, const vec2& v);
    friend vec2 operator *(const vec2& u, const vec2& v);
    friend vec2 operator /(const vec2& u, const vec2& v);
};

inline vec2 operator +(const vec2& u, const vec2& v)
{
    return vec2(u.x+v.x, u.y+v.y);
}

inline vec2 operator -(const vec2& u, const vec2& v)
{
    return vec2(u.x-v.x, u.y-v.y);
}

inline vec2 operator *(const vec2& u, const vec2& v)
{
    return vec2(u.x*v.x, u.y*v.y);
}

inline vec2 operator /(const vec2& u, const vec2& v)
{
    return vec2(u.x/v.x, u.y/v.y);
}

// vec3
struct vec3
{
    float x, y, z;

    inline static vec3 cross(const vec3& u, const vec3& v)
    {
        return vec3(u.y*v.z-v.y*u.z, u.z*v.x-v.z*u.x, u.x*v.y-v.x*u.y);
    }

    inline static float length(const vec3& u)
    {
        return sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
    }

    inline static vec3 normalize(const vec3& u)
    {
        float invmag = 1.0f/(length(u) + FLT_EPSILON);
        return vec3(u.x * invmag, u.y * invmag, u.z * invmag);
    }

    inline vec3()
    {
    }

    inline vec3(float x, float y, float z) 
      : x(x), y(y), z(z)
    {
    }

    friend vec3 operator +(const vec3& u, const vec3& v);
    friend vec3 operator -(const vec3& u, const vec3& v);
    friend vec3 operator *(const vec3& u, const vec3& v);
    friend vec3 operator /(const vec3& u, const vec3& v);
};

inline vec3 operator +(const vec3& u, const vec3& v)
{
    return vec3(u.x+v.x, u.y+v.y, u.z+v.z);
}

inline vec3 operator -(const vec3& u, const vec3& v)
{
    return vec3(u.x-v.x, u.y-v.y, u.z-v.z);
}

inline vec3 operator *(const vec3& u, const vec3& v)
{
    return vec3(u.x*v.x, u.y*v.y, u.z*v.z);
}

inline vec3 operator /(const vec3& u, const vec3& v)
{
    return vec3(u.x/v.x, u.y/v.y, u.z/v.z);
}


typedef union
{
	int data[4];
	struct { int x; int y; int z; int w; };
	struct { int r; int g; int b; int a; };
	struct { int vstart;  int vcount; int istart; int icount; };
} ivec4;


// vec 4
struct vec4
{
    float x, y, z, w;

    inline static vec4 cross(const vec4& u, const vec4& v)
    {
        return vec4(u.y*v.z-v.y*u.z, u.z*v.x-v.z*u.x, u.x*v.y-v.x*u.y, 0.0f);
    }

    inline static float length(const vec4& u)
    {
        return sqrt(u.x*u.x + u.y*u.y + u.z*u.z + u.w*u.w);
    }

    inline static vec4 normalize(const vec4& u)
    {
        float invmag = 1.0f/(length(u) + FLT_EPSILON);
        return vec4(u.x * invmag, u.y * invmag, u.z * invmag, u.w * invmag);
    }

    inline vec4()
    {
    }

    inline vec4(float x, float y, float z, float w) 
      : x(x), y(y), z(z), w(w)
    {
    }

    friend vec4 operator +(const vec4& u, const vec4& v);
    friend vec4 operator -(const vec4& u, const vec4& v);
    friend vec4 operator *(const vec4& u, const vec4& v);
    friend vec4 operator /(const vec4& u, const vec4& v);
};

inline vec4 operator +(const vec4& u, const vec4& v)
{
    return vec4(u.x+v.x, u.y+v.y, u.z+v.z, u.w+v.w);
}

inline vec4 operator -(const vec4& u, const vec4& v)
{
    return vec4(u.x-v.x, u.y-v.y, u.z-v.z, u.w-v.w);
}

inline vec4 operator *(const vec4& u, const vec4& v)
{
    return vec4(u.x*v.x, u.y*v.y, u.z*v.z, u.w*v.w);
}

inline vec4 operator /(const vec4& u, const vec4& v)
{
    return vec4(u.x/v.x, u.y/v.y, u.z/v.z, u.w/v.w);
}

} // namespace Forms

} // namespace Pt


#endif // include guard
