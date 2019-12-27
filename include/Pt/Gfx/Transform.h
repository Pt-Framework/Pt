/* Copyright (C) 2017 Marc Boris Duerner
   Copyright (C) 2017 Aloysius Indrayanto

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

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Gfx {

/** @brief 2D transformation matrix.
  */
class PT_GFX_API Transform
{
    public:
        Transform();

        Transform(double m11, double m12,
                  double m21, double m22,
                  double dx,  double dy);

        ~Transform();

        bool isIdentity() const;

        double m11() const;

        double m12() const;

        double m21() const;

        double m22() const;

        double dx() const;

        double dy() const;

        void reset();

        void set(double m11, double m12,
                 double m21, double m22,
                 double dx, double dy);

        void translate(double x, double y);

        void scale(double x, double y);

        void rotateDeg(double angle);

        void rotateRad(double angle);

        void shear(double sh, double sv);

        void shearX(double sh);

        void shearY(double sh);

        bool operator==(const Transform& t) const;

        bool operator!=(const Transform& t) const;

        Transform& operator*=(const Transform& t);

        Transform operator*(const Transform& t) const;

        PointF operator*(const PointF& p) const;

        SizeF operator*(const SizeF& p) const;

        double determinant() const;

        Transform inverted() const;

    private:
      typedef double MatrixData[2][3];

      void updateMatrix(const MatrixData& m);

    private:
        MatrixData _mdata;
        bool       _isIdentity;
};

} // namespace

} // namespace

#endif
