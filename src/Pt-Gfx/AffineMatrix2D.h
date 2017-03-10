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

#ifndef PT_GFX_AFFINEMATRIX2D_H
#define PT_GFX_AFFINEMATRIX2D_H

#include <Pt/Gfx/Math.h>


namespace Pt{
namespace Gfx{


class AffineMatrix2D {
    public:
        enum MatrixUpdateMode {
            Replace,         // M' = N
            MultiplyOnLeft,  // M' = N * M
            MultiplyOnRight  // M' = M * N
        };

    public:
        inline AffineMatrix2D();
        inline ~AffineMatrix2D();

        inline void identity();

        inline void translate(float x, float y, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void scaleAboutOrigin(float x, float y, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void rotateAboutOrigin(float deg, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void shearXDirection(float deg, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void shearYDirection(float deg, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutOrigin(MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutXAxis(MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutYAxis(MatrixUpdateMode mode = MultiplyOnLeft);

    private:
        typedef float MatrixData[3][3];

    private:
        inline void multiplyWith(const MatrixData& n, MatrixUpdateMode mode);

    private:
        MatrixData _mdata;
};


// ======================================================================================
// ===== Inlined Public Member Functions ================================================
// ======================================================================================

AffineMatrix2D::AffineMatrix2D()
{ identity(); }

AffineMatrix2D::~AffineMatrix2D()
{}

void AffineMatrix2D::identity()
{
    _mdata[0][0] = 1; _mdata[0][1] = 0;  _mdata[0][2] = 0;
    _mdata[1][0] = 0; _mdata[1][1] = 1;  _mdata[1][2] = 0;
    _mdata[2][0] = 0; _mdata[2][1] = 0;  _mdata[2][2] = 1;
}

void AffineMatrix2D::translate(float x, float y, MatrixUpdateMode mode)
{
    MatrixData n;

    n[0][0] = 1; n[0][1] = 0;  n[0][2] = x;
    n[1][0] = 0; n[1][1] = 1;  n[1][2] = y;
    n[2][0] = 0; n[2][1] = 0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::scaleAboutOrigin(float x, float y, MatrixUpdateMode mode)
{
    MatrixData n;

    n[0][0] = x; n[0][1] = 0;  n[0][2] = 0;
    n[1][0] = 0; n[1][1] = y;  n[1][2] = 0;
    n[2][0] = 0; n[2][1] = 0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::rotateAboutOrigin(float deg, MatrixUpdateMode mode)
{
    MatrixData n;

    const float s = Gfx::Math::fastSin(deg * Pt::Pi / 180);
    const float c = Gfx::Math::fastCos(deg * Pt::Pi / 180);

    n[0][0] =  c; n[0][1] = s;  n[0][2] = 0;
    n[1][0] = -s; n[1][1] = c;  n[1][2] = 0;
    n[2][0] =  0; n[2][1] = 0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::shearXDirection(float deg, MatrixUpdateMode mode)
{
    MatrixData n;

    const float t = Gfx::Math::fastSin(deg * Pt::Pi / 180) / Gfx::Math::fastCos(deg * Pt::Pi / 180);

    n[0][0] = 1; n[0][1] = t;  n[0][2] = 0;
    n[1][0] = 0; n[1][1] = 1;  n[1][2] = 0;
    n[2][0] = 0; n[2][1] = 0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::shearYDirection(float deg, MatrixUpdateMode mode)
{
    MatrixData n;

    const float t = Gfx::Math::fastSin(deg * Pt::Pi / 180) / Gfx::Math::fastCos(deg * Pt::Pi / 180);

    n[0][0] = 1; n[0][1] = 0;  n[0][2] = 0;
    n[1][0] = t; n[1][1] = 1;  n[1][2] = 0;
    n[2][0] = 0; n[2][1] = 0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::reflectAboutOrigin(MatrixUpdateMode mode)
{
    MatrixData n;

    n[0][0] = -1; n[0][1] =  0;  n[0][2] = 0;
    n[1][0] =  0; n[1][1] = -1;  n[1][2] = 0;
    n[2][0] =  0; n[2][1] =  0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::reflectAboutXAxis(MatrixUpdateMode mode)
{
    MatrixData n;

    n[0][0] = 1; n[0][1] =  0;  n[0][2] = 0;
    n[1][0] = 0; n[1][1] = -1;  n[1][2] = 0;
    n[2][0] = 0; n[2][1] =  0;  n[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::reflectAboutYAxis(MatrixUpdateMode mode)
{
    MatrixData n;

    n[0][0] = -1; n[0][1] = 0;  n[0][2] = 0;
    n[1][0] =  0; n[1][1] = 1;  n[1][2] = 0;
    n[2][0] =  0; n[2][1] = 0;  n[2][2] = 1;

    multiplyWith(n, mode);
}


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

void AffineMatrix2D::multiplyWith(const MatrixData& n, MatrixUpdateMode mode)
{
    MatrixData m;

    switch(mode) {
        case MultiplyOnLeft  :
            m[0][0] = n[0][0] * _mdata [0][0] + n[0][1] * _mdata [1][0] + n[0][2] * _mdata [2][0];
            m[0][1] = n[0][0] * _mdata [0][1] + n[0][1] * _mdata [1][1] + n[0][2] * _mdata [2][1];
            m[0][2] = n[0][0] * _mdata [0][2] + n[0][1] * _mdata [1][2] + n[0][2] * _mdata [2][2];
            m[1][0] = n[1][0] * _mdata [0][0] + n[1][1] * _mdata [1][0] + n[1][2] * _mdata [2][0];
            m[1][1] = n[1][0] * _mdata [0][1] + n[1][1] * _mdata [1][1] + n[1][2] * _mdata [2][1];
            m[1][2] = n[1][0] * _mdata [0][2] + n[1][1] * _mdata [1][2] + n[1][2] * _mdata [2][2];
            m[2][0] = n[2][0] * _mdata [0][0] + n[2][1] * _mdata [1][0] + n[2][2] * _mdata [2][0];
            m[2][1] = n[2][0] * _mdata [0][1] + n[2][1] * _mdata [1][1] + n[2][2] * _mdata [2][1];
            m[2][2] = n[2][0] * _mdata [0][2] + n[2][1] * _mdata [1][2] + n[2][2] * _mdata [2][2];
            memcpy(&_mdata[0], &m[0], sizeof(_mdata));
            break;

        case MultiplyOnRight :
            m[0][0] = _mdata[0][0] * n [0][0] + _mdata[0][1] * n [1][0] + _mdata[0][2] * n [2][0];
            m[0][1] = _mdata[0][0] * n [0][1] + _mdata[0][1] * n [1][1] + _mdata[0][2] * n [2][1];
            m[0][2] = _mdata[0][0] * n [0][2] + _mdata[0][1] * n [1][2] + _mdata[0][2] * n [2][2];
            m[1][0] = _mdata[1][0] * n [0][0] + _mdata[1][1] * n [1][0] + _mdata[1][2] * n [2][0];
            m[1][1] = _mdata[1][0] * n [0][1] + _mdata[1][1] * n [1][1] + _mdata[1][2] * n [2][1];
            m[1][2] = _mdata[1][0] * n [0][2] + _mdata[1][1] * n [1][2] + _mdata[1][2] * n [2][2];
            m[2][0] = _mdata[2][0] * n [0][0] + _mdata[2][1] * n [1][0] + _mdata[2][2] * n [2][0];
            m[2][1] = _mdata[2][0] * n [0][1] + _mdata[2][1] * n [1][1] + _mdata[2][2] * n [2][1];
            m[2][2] = _mdata[2][0] * n [0][2] + _mdata[2][1] * n [1][2] + _mdata[2][2] * n [2][2];
            memcpy(&_mdata[0], &m[0], sizeof(_mdata));
            break;

        default:
            memcpy(&_mdata[0], &n[0], sizeof(_mdata));
            break;
    }
}


} // namespace
} // namespace

#endif
