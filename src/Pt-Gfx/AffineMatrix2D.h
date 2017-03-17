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

        inline void getRaw(float m[3][3]) const;
        inline void updateUsingRaw(const float m[3][3], MatrixUpdateMode mode = MultiplyOnLeft);

        inline void operator=(const AffineMatrix2D& m);
        inline bool operator==(const AffineMatrix2D& m) const;
        inline bool operator!=(const AffineMatrix2D& m) const;

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
#if defined(PT_GFX_USE_AVX1) || defined(PT_GFX_USE_SSE1)
        union MatrixData {
            float  v[4][4];
            __m128 r[4];
        };
#else
        struct MatrixData {
            float v[3][3];
        };
#endif

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
{
    identity();

#if defined(PT_GFX_USE_AVX1) || defined(PT_GFX_USE_SSE1)
    _mdata.v[0][3] = 0; _mdata.v[1][3] = 0; _mdata.v[2][3] = 0;
    _mdata.v[3][0] = 0; _mdata.v[3][1] = 0; _mdata.v[3][2] = 0; _mdata.v[3][3] = 0;
#endif
}

AffineMatrix2D::~AffineMatrix2D()
{}

void AffineMatrix2D::clear()
{
    identity();
    _mstack.clear();
}

void AffineMatrix2D::identity()
{
    _mdata.v[0][0] = 1; _mdata.v[0][1] = 0; _mdata.v[0][2] = 0;
    _mdata.v[1][0] = 0; _mdata.v[1][1] = 1; _mdata.v[1][2] = 0;
    _mdata.v[2][0] = 0; _mdata.v[2][1] = 0; _mdata.v[2][2] = 1;
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
{
    MatrixData n;

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

void AffineMatrix2D::getRaw(float m[3][3]) const
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
{
    float r[3][3];
    m.getRaw(r);

    updateUsingRaw(r, Replace);
}

bool AffineMatrix2D::operator==(const AffineMatrix2D& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) == 0; }

bool AffineMatrix2D::operator!=(const AffineMatrix2D& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) != 0; }

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
    const float tx = _mdata.v[0][0] * x + _mdata.v[0][1] * y + _mdata.v[0][2];
    const float ty = _mdata.v[1][0] * x + _mdata.v[1][1] * y + _mdata.v[1][2];

    x = tx;
    y = ty;
}

void AffineMatrix2D::transformPoint(float& dx, float& dy, float sx, float sy)
{
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
   for(size_t i = 0; i < pointCount; i += 2) transformPoint(xy[i], xy[i + 1]);

}

void AffineMatrix2D::transformPoints(float* dxy, const float* sxy, size_t pointCount)
{

#if defined(PT_GFX_USE_AVX1)
    const float avxOneZero[8] = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

    const __m128 m0 = _mm_loadu_ps(_mdata.v[0]);
    const __m128 m1 = _mm_loadu_ps(_mdata.v[1]);
    const __m128 m2 = _mm_loadu_ps(_mdata.v[2]);

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    __m256 m00 = _mm256_castps128_ps256(m0);         // [ .. .. .. .. 00 01 02 03 ]
           m00 = _mm256_insertf128_ps  (m00, m0, 1); // [ 00 01 02 03 00 01 02 03 ]

    __m256 m11 = _mm256_castps128_ps256(m1);         // [ .. .. .. .. 10 11 12 13 ]
           m11 = _mm256_insertf128_ps  (m11, m1, 1); // [ 10 11 12 13 10 11 12 13 ]

    __m256 m22 = _mm256_castps128_ps256(m2);         // [ .. .. .. .. 20 21 22 23 ]
           m22 = _mm256_insertf128_ps  (m22, m2, 1); // [ 20 21 22 23 20 21 22 23 ]

    const size_t  pointCount8 = pointCount / 8;
    const __m256  coz8        = _mm256_loadu_ps(avxOneZero);

    for(size_t i = 0; i < pointCount8; ++i) {
        /// Load 8 floats from the source vector
        const __m256 s3210 = _mm256_loadu_ps  (sxy                                 ); // [ X0 Y0 X1 Y1 X2 Y2 X3 Y3 ]
        const __m256 s32   = _mm256_shuffle_ps(s3210, coz8, _MM_SHUFFLE(3, 2, 3, 2)); // [ X0 Y0 1  0  X2 Y2 1  0  ]
        const __m256 s10   = _mm256_shuffle_ps(s3210, coz8, _MM_SHUFFLE(1, 0, 1, 0)); // [ X1 Y1 1  0  X3 Y3 1  0  ]
        // Multiply them to the matrix's rows
        const __m256 r32_0 = _mm256_mul_ps(m00, s32);
        const __m256 r32_1 = _mm256_mul_ps(m11, s32);
        const __m256 r10_0 = _mm256_mul_ps(m00, s10);
        const __m256 r10_1 = _mm256_mul_ps(m11, s10);
        // Horizontal add the multiplication results
        const __m256 r32   = _mm256_hadd_ps(r32_0, r32_1);
        const __m256 r10   = _mm256_hadd_ps(r10_0, r10_1);
        const __m256 r3210 = _mm256_hadd_ps(r10,    r32);
        // Store 8 floats to the destination vector
        _mm256_storeu_ps(dxy, r3210);
/*
Point (11.000, 12.000) -> (10.500, 64.000) REF
Point (13.000, 24.000) -> (11.500, 88.000)
Point (25.000, 16.000) -> (17.500, 72.000)
Point (27.000, 28.000) -> (18.500, 96.000)



*/

        sxy += 8;
        dxy += 8;
    }

     _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    pointCount %= 8;

#endif

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

void AffineMatrix2D::transformPoints(PointF* xy, size_t pointCount)
{ for(size_t i = 0; i < pointCount; ++i) transformPoint(xy[i]); }

void AffineMatrix2D::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount)
{ for(size_t i = 0; i < pointCount; ++i) transformPoint(dxy[i], sxy[i]); }

// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

void AffineMatrix2D::multiplyWith(const MatrixData& n, MatrixUpdateMode mode)
{
    // Replace mode
    if(mode == Replace) {
        _mdata = n;
        return;
    }

    // Multiply mode
    const MatrixData* l;
    const MatrixData* r;

    if(mode == MultiplyOnLeft) {
        l = &n;
        r = &_mdata;
    }
    else { // MultiplyOnRight
        l = &_mdata;
        r = &n;
    }

#if defined(PT_GFX_USE_AVX1)

     _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    __m128 out0x =                    _mm_mul_ps(_mm_broadcast_ss(&l->v[0][0]), r->r[0])  ;
           out0x = _mm_add_ps( out0x, _mm_mul_ps(_mm_broadcast_ss(&l->v[0][1]), r->r[1]) );
           out0x = _mm_add_ps( out0x, _mm_mul_ps(_mm_broadcast_ss(&l->v[0][2]), r->r[2]) );

    __m128 out1x =                    _mm_mul_ps(_mm_broadcast_ss(&l->v[1][0]), r->r[0])  ;
           out1x = _mm_add_ps( out1x, _mm_mul_ps(_mm_broadcast_ss(&l->v[1][1]), r->r[1]) );
           out1x = _mm_add_ps( out1x, _mm_mul_ps(_mm_broadcast_ss(&l->v[1][2]), r->r[2]) );

    __m128 out2x =                    _mm_mul_ps(_mm_broadcast_ss(&l->v[2][0]), r->r[0])  ;
           out2x = _mm_add_ps( out2x, _mm_mul_ps(_mm_broadcast_ss(&l->v[2][1]), r->r[1]) );
           out2x = _mm_add_ps( out2x, _mm_mul_ps(_mm_broadcast_ss(&l->v[2][2]), r->r[2]) );

    _mm_storeu_ps(_mdata.v[0], out0x);
    _mm_storeu_ps(_mdata.v[1], out1x);
    _mm_storeu_ps(_mdata.v[2], out2x);

     _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

#else

    MatrixData  o;

    o.v[0][0] = l->v[0][0] * r->v[0][0] + l->v[0][1] * r->v[1][0] + l->v[0][2] * r->v[2][0];
    o.v[0][1] = l->v[0][0] * r->v[0][1] + l->v[0][1] * r->v[1][1] + l->v[0][2] * r->v[2][1];
    o.v[0][2] = l->v[0][0] * r->v[0][2] + l->v[0][1] * r->v[1][2] + l->v[0][2] * r->v[2][2];

    o.v[1][0] = l->v[1][0] * r->v[0][0] + l->v[1][1] * r->v[1][0] + l->v[1][2] * r->v[2][0];
    o.v[1][1] = l->v[1][0] * r->v[0][1] + l->v[1][1] * r->v[1][1] + l->v[1][2] * r->v[2][1];
    o.v[1][2] = l->v[1][0] * r->v[0][2] + l->v[1][1] * r->v[1][2] + l->v[1][2] * r->v[2][2];

    o.v[2][0] = l->v[2][0] * r->v[0][0] + l->v[2][1] * r->v[1][0] + l->v[2][2] * r->v[2][0];
    o.v[2][1] = l->v[2][0] * r->v[0][1] + l->v[2][1] * r->v[1][1] + l->v[2][2] * r->v[2][1];
    o.v[2][2] = l->v[2][0] * r->v[0][2] + l->v[2][1] * r->v[1][2] + l->v[2][2] * r->v[2][2];

    _mdata = o;

#endif
}


} // namespace
} // namespace

#endif
