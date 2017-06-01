/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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
  MA 02110-1301 USA
*/

#ifndef PT_GFX_TRANSFORM_H
#define PT_GFX_TRANSFORM_H

#include <cstring>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Gfx{


/** @brief A transform class for performing 2D transformation on points.
  */
class PT_GFX_API Transform 
{
    public:
        Transform();

        ~Transform();

        bool isIdentity() const
        {
         return _isIdentity ;
        }

        // reset the matrix to the identity matrix
        void reset();

        // set the translation factors in the _matrix 
        void translate(double x, double y);
        
        // set the sacling factors in the _matrix
        void scale(double x, double y);

        // set the rotaion factors in the _matrix 
        // This uses an angle in degree.
        //
        // angle = 30.0;
        // radians = angle * PI/180;
        void rotateDeg(double angle);

        // set the rotaion factors in the _matrix 
        // This uses an angle in radians.
        void rotateRad(double angle);

        // set the horizontal and vertical shear factors in the _matrix 
        void shear(double sh, double sv);

        void shearX(double sh);

        void shearY(double sh);

        double m11() const
        {
          return _mdata[0][0];
        }

        double m12() const
        {
          return _mdata[0][1];
        }

        double m21() const
        {
          return _mdata[1][0];
        }

        double m22() const
        {
          return _mdata[1][1];
        }

        double dx() const
        {
          return _mdata[0][2];
        }

        double dy() const
        {
          return _mdata[1][2];
        }

        
        // sets the matrix fields
        void set( double m11, double m12, double m21, double m22,
                  double dx, double dy )
        {

          _mdata[0][0] = m11; 
          _mdata[0][1] = m12; 
          _mdata[0][2] = dx;
          _mdata[1][0] = m21; 
          _mdata[1][1] = m22; 
          _mdata[1][2] = dy;            
            
          _isIdentity = ( _mdata[0][0] == 1 && _mdata[0][1] == 0 && _mdata[0][2] == 0 &&
              _mdata[1][0] == 0 && _mdata[1][1] == 1 && _mdata[1][2] == 0);
        }

        Transform operator*(const Transform& t) const;
        
        PointF operator*(const PointF& p) const;
        
        SizeF operator*(const SizeF& p) const;

        Transform& operator*=(const Transform& t);

        bool operator==(const Transform& t) const;

        bool operator!=(const Transform& t) const;

    private:
      typedef double MatrixData[2][3];

      void updateMatrix(const MatrixData& n);
  
    private:
        MatrixData  _mdata;
        bool    _isIdentity;
};



} // namespace
} // namespace

#endif
