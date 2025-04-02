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

#include "Matrix.h"

#include <cfloat>
#include <cmath>

namespace Pt {

namespace Forms {

Matrix::Matrix()
{
  *this = Matrix
    (
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
}


Matrix::Matrix(
			          float v00, float v01, float v02, float v03,
			          float v10, float v11, float v12, float v13,
			          float v20, float v21, float v22, float v23,
			          float v30, float v31, float v32, float v33
		          )
{
  _m[0] = v00; _m[4] = v01; _m[8]  = v02; _m[12] = v03;
  _m[1] = v10; _m[5] = v11; _m[9]  = v12; _m[13] = v13;
	_m[2] = v20; _m[6] = v21; _m[10] = v22; _m[14] = v23;
	_m[3] = v30; _m[7] = v31; _m[11] = v32; _m[15] = v33;
}

Matrix::~Matrix()
{
}


const Matrix Matrix::operator*( const Matrix& mat ) const
{
  return Matrix
  (
    _m[0] * mat._m[0]  + _m[4] * mat._m[1]  + _m[8]  * mat._m[2]  + _m[12] * mat._m[3], 
    _m[0] * mat._m[4]  + _m[4] * mat._m[5]  + _m[8]  * mat._m[6]  + _m[12] * mat._m[7], 
    _m[0] * mat._m[8]  + _m[4] * mat._m[9]  + _m[8]  * mat._m[10] + _m[12] * mat._m[11], 
    _m[0] * mat._m[12] + _m[4] * mat._m[13] + _m[8]  * mat._m[14] + _m[12] * mat._m[15],
    _m[1] * mat._m[0]  + _m[5] * mat._m[1]  + _m[9]  * mat._m[2]  + _m[13] * mat._m[3], 
    _m[1] * mat._m[4]  + _m[5] * mat._m[5]  + _m[9]  * mat._m[6]  + _m[13] * mat._m[7], 
    _m[1] * mat._m[8]  + _m[5] * mat._m[9]  + _m[9]  * mat._m[10] + _m[13] * mat._m[11], 
    _m[1] * mat._m[12] + _m[5] * mat._m[13] + _m[9]  * mat._m[14] + _m[13] * mat._m[15],
    _m[2] * mat._m[0]  + _m[6] * mat._m[1]  + _m[10] * mat._m[2]  + _m[14] * mat._m[3], 
    _m[2] * mat._m[4]  + _m[6] * mat._m[5]  + _m[10] * mat._m[6]  + _m[14] * mat._m[7], 
    _m[2] * mat._m[8]  + _m[6] * mat._m[9]  + _m[10] * mat._m[10] + _m[14] * mat._m[11], 
    _m[2] * mat._m[12] + _m[6] * mat._m[13] + _m[10] * mat._m[14] + _m[14] * mat._m[15],
    _m[3] * mat._m[0]  + _m[7] * mat._m[1]  + _m[11] * mat._m[2]  + _m[15] * mat._m[3], 
    _m[3] * mat._m[4]  + _m[7] * mat._m[5]  + _m[11] * mat._m[6]  + _m[15] * mat._m[7], 
    _m[3] * mat._m[8]  + _m[7] * mat._m[9]  + _m[11] * mat._m[10] + _m[15] * mat._m[11], 
    _m[3] * mat._m[12] + _m[7] * mat._m[13] + _m[11] * mat._m[14] + _m[15] * mat._m[15]
  );
}


const vec3 Matrix::operator*( const vec3& v ) const
{
  return vec3
  (
    _m[0] * v.x + _m[4] * v.y + _m[8]  * v.z + _m[12],
    _m[1] * v.x + _m[5] * v.y + _m[9]  * v.z + _m[13],
    _m[2] * v.x + _m[6] * v.y + _m[10] * v.z + _m[14]
  );
}


const vec4 Matrix::operator*( const vec4& v ) const
{
  return vec4
  (
    _m[0] * v.x + _m[4] * v.y + _m[8]  * v.z + _m[12] * v.w,
    _m[1] * v.x + _m[5] * v.y + _m[9]  * v.z + _m[13] * v.w,
    _m[2] * v.x + _m[6] * v.y + _m[10] * v.z + _m[14] * v.w,
    _m[3] * v.x + _m[7] * v.y + _m[11] * v.z + _m[15] * v.w
  );
}


Matrix& Matrix::translate( const vec3& v )
{
  return *this = *this * Matrix
  (
    1.0f, 0.0f, 0.0f, v.x,
    0.0f, 1.0f, 0.0f, v.y,
    0.0f, 0.0f, 1.0f, v.z,
    0.0f, 0.0f, 0.0f, 1.0f
  );
}


Matrix& Matrix::scale( const vec3& v )
{
  return *this = *this * Matrix
  (
    v.x,  0.0f, 0.0f, 0.0f,
    0.0f, v.y,  0.0f, 0.0f,
    0.0f, 0.0f, v.z,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
}


Matrix& Matrix::rotateX( float deg )
{
  float rad = deg/180.0f * 3.141593f;

  return *this = *this * Matrix
  (
    1.0f, 0.0f,        0.0f,         0.0f,
    0.0f, cos( rad ),  -sin( rad ),  0.0f,
    0.0f, sin( rad ),  cos( rad ),   0.0f,
    0.0f, 0.0f,        0.0f,         1.0f
  );
}


Matrix& Matrix::rotateY( float deg )
{
  float rad = deg/180.0f * 3.141593f;

  return *this = *this * Matrix
  (
    cos( rad ),  0.0f, sin( rad ), 0.0f,
    0.0f,        1.0f, 0.0f,       0.0f,
    -sin( rad ), 0.0f, cos( rad ), 0.0f,
    0.0f,        0.0f, 0.0f,       1.0f
  );
}


Matrix& Matrix::rotate( const vec3& axis, float deg )
{
  float rad = deg/180.0f * 3.141593f;
  float s = sin( rad );
  float c = cos( rad );
  float t = 1 - c;
  
  vec3 a = vec3::normalize(axis);

  return *this = *this * Matrix
  (
    a.x * a.x * t + c,        a.x * a.y * t - a.z * s,  a.x * a.z * t + a.y * s,  0,    
    a.y * a.x * t + a.z * s,  a.y * a.y * t + c,        a.y * a.z * t - a.x * s,  0,
    a.z * a.x * t - a.y * s,  a.z * a.y * t + a.x * s,  a.z * a.z * t + c,        0,
    0,                        0,                        0,                        1
  );
}


Matrix& Matrix::identity()
{
  return *this = *this * Matrix
  (
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );
}


Matrix Matrix::perspective( float fovY, float aspect, float zNear, float zFar )
{
  float rad = fovY / 180.0f * 3.141593f * 0.5f;
  float cotan = cos(rad) / sin(rad);
  float deltaZ = zNear - zFar;

  return Matrix
         (
            cotan/aspect,
            0.0f,
            0.0f,
            0.0f,

            0.0f,
            cotan,
            0.0f,
            0.0f,

            0.0f,
            0.0f,
            (zFar + zNear) / deltaZ,
            -1.0f,

            0.0f,
            0.0f,
            2.0f * zNear * zFar / deltaZ,
            0.0f
         );
}


Matrix Matrix::ortho( float left, float right, float bottom, float top, float zNear, float zFar )
{
  Matrix res;
  res._m[0]   = 2.0f / ( right - left );
  res._m[1]   = 0.0f;
  res._m[2]   = 0.0f;
  res._m[3]   = 0.0f;

  res._m[4]   = 0.0f;
  res._m[5]   = 2.0f / ( top - bottom );
  res._m[6]   = 0.0f;
  res._m[7]   = 0.0f;

  res._m[8]   = 0.0f;
  res._m[9]   = 0.0f;
  res._m[10]  = -2.0f / ( zFar - zNear );
  res._m[11]  = 0.0f;

  res._m[12]  = - ( right + left ) / ( right - left );
  res._m[13]  = - ( top + bottom ) / ( top - bottom );
  res._m[14]  = - ( zFar + zNear )  / ( zFar - zNear );
  res._m[15]  = 1.0f;

  return res;
}


Matrix Matrix::lookAt( const vec3& eye, const vec3& target, const vec3& up )
{
  vec3 f = vec3::normalize(target - eye);
  vec3 s = vec3::normalize(vec3::cross(f, up));
  vec3 u = vec3::normalize(vec3::cross(s, f));

  Matrix res
  (
    s.x,
    u.x,
    -f.x,
    0.0f,

    s.y,
    u.y,
    -f.y,
    0.0f,

    s.z,
    u.z,
    -f.z,
    0.0f,

    0.0f,
    0.0f,
    0.0f,
    1.0f
  );

  vec4 t = res * vec4(-eye.x, -eye.y, -eye.z, 1.0f);
  res._m[13] = t.x;
  res._m[14] = t.y;
  res._m[15] = t.z;


  return res;
}

} // namespace Forms

} // namespace Pt
