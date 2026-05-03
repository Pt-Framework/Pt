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

/** @brief Affine transform for drawing coordinates.
    @ingroup Drawing

    Transform stores the matrix used to map logical drawing coordinates to the
    coordinate system of the target. It can be composed from translation,
    scaling, rotation and shear operations and is used by painters, paths and
    text drawing.
*/
class PT_GFX_API Transform
{
    public:
        /** @brief Constructs the identity transform.
        */
        Transform();

        /** @brief Constructs a transform from explicit matrix components.
        */
        Transform(double m11, double m12,
                  double m21, double m22,
                  double dx,  double dy);

        /** @brief Destroys the transform.
        */
        ~Transform();

        /** @brief Returns true if the transform is the identity matrix.
        */
        bool isIdentity() const;

        /** @brief Returns true if the transform is affine.
        */
        bool isAffine() const;
        
        /** @brief Returns true if the matrix is only translating and scaling.
        */
        bool isSimple() const;

        /** @brief Returns the first row, first column coefficient.
        */
        double m11() const;

        /** @brief Returns the first row, second column coefficient.
        */
        double m12() const;

        /** @brief Returns the second row, first column coefficient.
        */
        double m21() const;

        /** @brief Returns the second row, second column coefficient.
        */
        double m22() const;

        /** @brief Returns the horizontal translation.
        */
        double dx() const;

        /** @brief Returns the vertical translation.
        */
        double dy() const;

        /** @brief Resets the transform to identity.
        */
        void reset();

        /** @brief Replaces the matrix coefficients.
        */
        void set(double m11, double m12,
                 double m21, double m22,
                 double dx, double dy);

        /** @brief Appends a translation.
        */
        void translate(double x, double y);

        /** @brief Appends a scaling.
        */
        void scale(double x, double y);

        /** @brief Appends a rotation specified in degrees.
        */
        void rotateDeg(double angle);

        /** @brief Appends a rotation specified in radians.
        */
        void rotateRad(double angle);

        /** @brief Appends horizontal and vertical shearing.
        */
        void shear(double sh, double sv);

        /** @brief Appends horizontal shearing.
        */
        void shearX(double sh);

        /** @brief Appends vertical shearing.
        */
        void shearY(double sh);

        /** @brief Returns true if both transforms are equal.
        */
        bool operator==(const Transform& t) const;

        /** @brief Returns true if both transforms are different.
        */
        bool operator!=(const Transform& t) const;

        /** @brief Appends another transform in place.
        */
        Transform& operator*=(const Transform& t);

        /** @brief Returns the composition of two transforms.
        */
        Transform operator*(const Transform& t) const;

        /** @brief Transforms a point.
        */
        PointF operator*(const PointF& p) const;

        /** @brief Transforms a size.
        */
        SizeF operator*(const SizeF& p) const;

        /** @brief Returns the matrix determinant.
        */
        double determinant() const;

        /** @brief Returns true if the transform can be inverted.
        */
        bool isInvertible() const;

        /** @brief Returns the inverse transform.
        */
        Transform inverted() const;

    private:
      typedef double MatrixData[2][3];

      void concat(const MatrixData& m);

      void updateIdentity();

    private:
        MatrixData _mdata;
        bool       _isIdentity;
};

} // namespace

} // namespace

#endif
