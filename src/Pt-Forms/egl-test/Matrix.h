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

#ifndef Pt_Forms_Matrix_H
#define Pt_Forms_Matrix_H

#include "Vector.h"

namespace Pt {

namespace Forms {

class Matrix 
{
public:
  Matrix();

  Matrix(
			    float v00, float v01, float v02, float v03,
			    float v10, float v11, float v12, float v13,
			    float v20, float v21, float v22, float v23,
			    float v30, float v31, float v32, float v33
		    );

  ~Matrix();

  const Matrix  operator*( const Matrix& mat ) const;
  const vec3    operator*( const vec3& v ) const;
  const vec4    operator*( const vec4& v ) const;

  Matrix& translate( const vec3& v );
  Matrix& scale( const vec3& v );

  Matrix& rotateX( float deg );
  Matrix& rotateY( float deg );
  Matrix& rotate( const vec3& axis, float deg );
  Matrix& identity();

  const float* get() const
  { return _m; }

  static Matrix perspective( float fovY, float aspect, float zNear, float zFar );
  static Matrix ortho( float left, float right, float bottom, float top, float zNear, float zFar );
  static Matrix lookAt( const vec3& eye, const vec3& target, const vec3& up );

public: // TODO: refactor
  float _m[16];
};

} // namespace Forms

} // namespace Pt


#endif // include guard