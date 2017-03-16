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

#include <vector>

#include <Pt/Gfx/Math.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/SIMDConfig.h>


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

        inline void clear();

        inline void identity();

        inline void translate(float x, float y, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void scaleAboutOrigin(float x, float y, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void rotateAboutOrigin(float deg, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void shearXDirection(float deg, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void shearYDirection(float deg, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void reflectAboutOrigin(MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutXAxis(MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutYAxis(MatrixUpdateMode mode = MultiplyOnLeft);

        inline void getRaw(float m[3][3]);
        inline void updateUsingRaw(const float m[3][3], MatrixUpdateMode mode = MultiplyOnLeft);

        inline void operator=(const AffineMatrix2D& m);
        inline bool operator==(const AffineMatrix2D& m) const;

        inline void push();
        inline bool pop();

        inline void transformPoint(float& x, float &y);
        inline void transformPoint(float& dx, float& dy, float sx, float sy);

        inline void transformPoint(PointF& p);
        inline void transformPoint(PointF& dp, const PointF& sp);

        inline void transformPoints(float* xy, size_t pointCount);
        inline void transformPoints(float* dxy, const float* sxy, size_t pointCount);

        inline void transformPoints(PointF* xy, size_t pointCount);
        inline void transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount);

    private:
        struct MatrixData {
            float v[3][3];
        };

    private:
        inline void multiplyWith(const MatrixData& n, MatrixUpdateMode mode);

    private:
        MatrixData              _mdata;
        std::vector<MatrixData> _mstack;
};


// ======================================================================================
// ===== Inlined Public Member Functions ================================================
// ======================================================================================

AffineMatrix2D::AffineMatrix2D()
{ identity(); }

AffineMatrix2D::~AffineMatrix2D()
{}

void AffineMatrix2D::clear()
{
    identity();
    _mstack.clear();
}

void AffineMatrix2D::identity()
{
    _mdata.v[0][0] = 1; _mdata.v[0][1] = 0;  _mdata.v[0][2] = 0;
    _mdata.v[1][0] = 0; _mdata.v[1][1] = 1;  _mdata.v[1][2] = 0;
    _mdata.v[2][0] = 0; _mdata.v[2][1] = 0;  _mdata.v[2][2] = 1;
}

void AffineMatrix2D::translate(float x, float y, MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = 1; n.v[0][1] = 0;  n.v[0][2] = x;
    n.v[1][0] = 0; n.v[1][1] = 1;  n.v[1][2] = y;
    n.v[2][0] = 0; n.v[2][1] = 0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::scaleAboutOrigin(float x, float y, MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = x; n.v[0][1] = 0;  n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = y;  n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::rotateAboutOrigin(float deg, MatrixUpdateMode mode)
{    MatrixData n;

    const float r = deg * Pt::Pi / 180;
    const float s = Gfx::Math::fastSin(r);
    const float c = Gfx::Math::fastCos(r);

    n.v[0][0] =  c; n.v[0][1] = s;  n.v[0][2] = 0;
    n.v[1][0] = -s; n.v[1][1] = c;  n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::shearXDirection(float deg, MatrixUpdateMode mode)
{
    MatrixData n;

    const float r = deg * Pt::Pi / 180;
    const float t = Gfx::Math::fastSin(r) / Gfx::Math::fastCos(r);

    n.v[0][0] = 1; n.v[0][1] = t;  n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = 1;  n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::shearYDirection(float deg, MatrixUpdateMode mode)
{
    MatrixData n;

    const float r = deg * Pt::Pi / 180;
    const float t = Gfx::Math::fastSin(r) / Gfx::Math::fastCos(r);

    n.v[0][0] = 1; n.v[0][1] = 0;  n.v[0][2] = 0;
    n.v[1][0] = t; n.v[1][1] = 1;  n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::reflectAboutOrigin(MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = -1; n.v[0][1] =  0;  n.v[0][2] = 0;
    n.v[1][0] =  0; n.v[1][1] = -1;  n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] =  0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::reflectAboutXAxis(MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = 1; n.v[0][1] =  0;  n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = -1;  n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] =  0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::reflectAboutYAxis(MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = -1; n.v[0][1] = 0;  n.v[0][2] = 0;
    n.v[1][0] =  0; n.v[1][1] = 1;  n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0;  n.v[2][2] = 1;

    multiplyWith(n, mode);
}

void AffineMatrix2D::getRaw(float m[3][3])
{
    m[0][0] = _mdata.v[0][0]; m[0][1] = _mdata.v[0][1]; m[0][2] = _mdata.v[0][2];
    m[1][0] = _mdata.v[1][0]; m[1][1] = _mdata.v[1][1]; m[1][2] = _mdata.v[1][2];
    m[2][0] = _mdata.v[2][0]; m[2][1] = _mdata.v[2][1]; m[2][2] = _mdata.v[2][2];
}

void AffineMatrix2D::updateUsingRaw(const float m[3][3], MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = m[0][0]; n.v[0][1] = m[0][1]; n.v[0][2] = m[0][2];
    n.v[1][0] = m[1][0]; n.v[1][1] = m[1][1]; n.v[1][2] = m[1][2];
    n.v[2][0] = m[2][0]; n.v[2][1] = m[2][1]; n.v[2][2] = m[2][2];

    multiplyWith(n, mode);
}

void AffineMatrix2D::operator=(const AffineMatrix2D& m)
{ updateUsingRaw(m._mdata.v, Replace); }

bool AffineMatrix2D::operator==(const AffineMatrix2D& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) == 0; }

void AffineMatrix2D::push()
{ _mstack.push_back(_mdata); }

bool AffineMatrix2D::pop()
{
    if(_mstack.empty()) return false;

    _mdata = _mstack.back();
    _mstack.pop_back();

    return true;
}

void AffineMatrix2D::transformPoint(float& x, float &y)
{
    // ### TODO: SIMD !!! ###

    const float tx = _mdata.v[0][0] * x + _mdata.v[0][1] * y + _mdata.v[0][2];
    const float ty = _mdata.v[1][0] * x + _mdata.v[1][1] * y + _mdata.v[1][2];

    x = tx;
    y = ty;
}

void AffineMatrix2D::transformPoint(float& dx, float& dy, float sx, float sy)
{
    // ### TODO: SIMD !!! ###

    dx = _mdata.v[0][0] * sx + _mdata.v[0][1] * sy + _mdata.v[0][2];
    dy = _mdata.v[1][0] * sx + _mdata.v[1][1] * sy + _mdata.v[1][2];
}

void AffineMatrix2D::transformPoint(PointF& p)
{
    float x = p.x();
    float y = p.x();

    transformPoint(x, y);
    p.set(x, y);
}

void AffineMatrix2D::transformPoint(PointF& dp, const PointF& sp)
{
    float x = sp.x();
    float y = sp.x();

    transformPoint(x, y);
    dp.set(x, y);
}

void AffineMatrix2D::transformPoints(float* xy, size_t pointCount)
{
    // ### TODO ###
}

void AffineMatrix2D::transformPoints(float* dxy, const float* sxy, size_t pointCount)
{
    // ### TODO ###
}

void AffineMatrix2D::transformPoints(PointF* xy, size_t pointCount)
{
    // ### TODO ###
}

void AffineMatrix2D::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount)
{
    // ### TODO ###
}


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

void AffineMatrix2D::multiplyWith(const MatrixData& n, MatrixUpdateMode mode)
{
    // ### TODO: SIMD !!! ###

    MatrixData m;

    switch(mode) {
        case MultiplyOnLeft  :
            m.v[0][0] = n.v[0][0] * _mdata.v[0][0] + n.v[0][1] * _mdata.v[1][0] + n.v[0][2] * _mdata.v[2][0];
            m.v[0][1] = n.v[0][0] * _mdata.v[0][1] + n.v[0][1] * _mdata.v[1][1] + n.v[0][2] * _mdata.v[2][1];
            m.v[0][2] = n.v[0][0] * _mdata.v[0][2] + n.v[0][1] * _mdata.v[1][2] + n.v[0][2] * _mdata.v[2][2];
            m.v[1][0] = n.v[1][0] * _mdata.v[0][0] + n.v[1][1] * _mdata.v[1][0] + n.v[1][2] * _mdata.v[2][0];
            m.v[1][1] = n.v[1][0] * _mdata.v[0][1] + n.v[1][1] * _mdata.v[1][1] + n.v[1][2] * _mdata.v[2][1];
            m.v[1][2] = n.v[1][0] * _mdata.v[0][2] + n.v[1][1] * _mdata.v[1][2] + n.v[1][2] * _mdata.v[2][2];
            m.v[2][0] = n.v[2][0] * _mdata.v[0][0] + n.v[2][1] * _mdata.v[1][0] + n.v[2][2] * _mdata.v[2][0];
            m.v[2][1] = n.v[2][0] * _mdata.v[0][1] + n.v[2][1] * _mdata.v[1][1] + n.v[2][2] * _mdata.v[2][1];
            m.v[2][2] = n.v[2][0] * _mdata.v[0][2] + n.v[2][1] * _mdata.v[1][2] + n.v[2][2] * _mdata.v[2][2];
            _mdata = m;
            break;

        case MultiplyOnRight :
            m.v[0][0] = _mdata.v[0][0] * n.v[0][0] + _mdata.v[0][1] * n.v[1][0] + _mdata.v[0][2] * n.v[2][0];
            m.v[0][1] = _mdata.v[0][0] * n.v[0][1] + _mdata.v[0][1] * n.v[1][1] + _mdata.v[0][2] * n.v[2][1];
            m.v[0][2] = _mdata.v[0][0] * n.v[0][2] + _mdata.v[0][1] * n.v[1][2] + _mdata.v[0][2] * n.v[2][2];
            m.v[1][0] = _mdata.v[1][0] * n.v[0][0] + _mdata.v[1][1] * n.v[1][0] + _mdata.v[1][2] * n.v[2][0];
            m.v[1][1] = _mdata.v[1][0] * n.v[0][1] + _mdata.v[1][1] * n.v[1][1] + _mdata.v[1][2] * n.v[2][1];
            m.v[1][2] = _mdata.v[1][0] * n.v[0][2] + _mdata.v[1][1] * n.v[1][2] + _mdata.v[1][2] * n.v[2][2];
            m.v[2][0] = _mdata.v[2][0] * n.v[0][0] + _mdata.v[2][1] * n.v[1][0] + _mdata.v[2][2] * n.v[2][0];
            m.v[2][1] = _mdata.v[2][0] * n.v[0][1] + _mdata.v[2][1] * n.v[1][1] + _mdata.v[2][2] * n.v[2][1];
            m.v[2][2] = _mdata.v[2][0] * n.v[0][2] + _mdata.v[2][1] * n.v[1][2] + _mdata.v[2][2] * n.v[2][2];
            _mdata = m;
            break;

        default: // Replace
            _mdata = n;
            break;
    }
}


} // namespace
} // namespace

#endif
