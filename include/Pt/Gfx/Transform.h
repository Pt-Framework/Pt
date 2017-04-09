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

#include <Pt/Gfx/Math.h>
#include <Pt/Gfx/Painter.h>


namespace Pt{
namespace Gfx{


#if defined(PT_GFX_USE_AVX1)

// AVX constants
static const __m256 avxOneZeroF = _mm256_set_ps(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
static const __m256 avxMaxCordF = _mm256_set1_ps(Painter::MaximumCoordinate);

#endif

#if defined(PT_GFX_USE_NEON)

// NEON constants
static const float32x4_t neonOneZeroF = NEON_SET_FLT32X4(0.0f, 1.0f, 0.0f, 1.0f);
static const float32x4_t neonMaxCordF = vdupq_n_f32(Painter::MaximumCoordinate);

#endif


// ======================================================================================
// ===== MatrixData Implementation ======================================================
// ======================================================================================
#if defined(PT_GFX_USE_AVX1)

template <typename T>
union BasicMatrixData {
    T v[4][4];
};

template <>
union BasicMatrixData<float> {
    float  v[4][4];
    __m128 r[4];
};

template <>
union BasicMatrixData<double> {
    double  v[4][4];
    __m256d r[4];
};

#elif defined(PT_GFX_USE_NEON)

template <typename T>
union BasicMatrixData {
    T           v[4][4];
    float32x4_t r[4];
};

#else

template <typename T>
struct BasicMatrixData {
    T v[3][3];
};

#endif


/** @brief A transform class for performing 2D transformation on points.
  */
template <typename T>
class PT_GFX_API BasicTransform {
    public:
        typedef T ValueT;

    public:
        inline BasicTransform();
        inline BasicTransform(const BasicTransform& m);

        inline ~BasicTransform();

        inline bool isIdentity() const;

        inline void identity();

        inline void translate(T x, T y, bool replaceInsteadOfCombine = false);

        inline void scale(T x, T y, bool replaceInsteadOfCombine = false);

        inline void rotate(T deg, bool replaceInsteadOfCombine = false);

        inline void shearX(T deg, bool replaceInsteadOfCombine = false);
        inline void shearY(T deg, bool replaceInsteadOfCombine = false);

        inline void getRaw(T m[3][3]) const;
        inline void setRaw(const T m[3][3]);

        inline const BasicTransform& operator=(const BasicTransform& m);
        inline const BasicTransform operator*(const BasicTransform& m) const;

        inline bool operator==(const BasicTransform& m) const;
        inline bool operator!=(const BasicTransform& m) const;

        inline void transformPoint(T& dx, T& dy, T sx, T sy) const;
        inline void transformPoint(T& x, T &y) const;

        inline void transformPoints(T* dxy, const T* sxy, size_t pointCount) const;
        inline void transformPoints(T* xy, size_t pointCount) const;

        inline void transformPoint(PointF& dp, const PointF& sp) const;
        inline void transformPoint(PointF& p) const;

        inline void transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const;
        inline void transformPoints(PointF* xy, size_t pointCount) const;

    private:
        // Matrix data
        typedef BasicMatrixData<T> MatrixData;

    private:
        inline void updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine);

    private:
        MatrixData _mdata;
        bool       _isIdentity;
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

template <typename T>
inline void BasicTransform<T>::updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine)
{
    // Check if we need to simply replace the matrix
    if(_isIdentity || replaceInsteadOfCombine) {
        _mdata = n;
        return;
    }

    // Multiply using the mode M' = N * M
    const MatrixData* l = &n;
    const MatrixData* r = &_mdata;
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
}


// ======================================================================================
// ===== Inlined Public Member Functions ================================================
// ======================================================================================

template <typename T>
inline BasicTransform<T>::BasicTransform()
{
    identity();

#if defined(PT_GFX_USE_AVX1) || defined(PT_GFX_USE_NEON)
    _mdata.v[0][3] = 0; _mdata.v[1][3] = 0; _mdata.v[2][3] = 0;
    _mdata.v[3][0] = 0; _mdata.v[3][1] = 0; _mdata.v[3][2] = 0; _mdata.v[3][3] = 0;
#endif
}

template <typename T>
inline BasicTransform<T>::BasicTransform(const BasicTransform<T>& m)
{ *this = m; }

template <typename T>
inline BasicTransform<T>::~BasicTransform()
{}

template <typename T>
inline bool BasicTransform<T>::isIdentity() const
{ return _isIdentity; }

template <typename T>
inline void BasicTransform<T>::identity()
{
    _mdata.v[0][0] = 1; _mdata.v[0][1] = 0; _mdata.v[0][2] = 0;
    _mdata.v[1][0] = 0; _mdata.v[1][1] = 1; _mdata.v[1][2] = 0;
    _mdata.v[2][0] = 0; _mdata.v[2][1] = 0; _mdata.v[2][2] = 1;

    _isIdentity = true;
}

template <typename T>
inline void BasicTransform<T>::translate(T x, T y, bool replaceInsteadOfCombine)
{
    if(x == 0 && y == 0) return;

    MatrixData n;

    n.v[0][0] = 1; n.v[0][1] = 0; n.v[0][2] = x;
    n.v[1][0] = 0; n.v[1][1] = 1; n.v[1][2] = y;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
inline void BasicTransform<T>::scale(T x, T y, bool replaceInsteadOfCombine)
{
    if(x == 1 && y == 1) return;

    MatrixData n;

    n.v[0][0] = x; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = y; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
inline void BasicTransform<T>::rotate(T deg, bool replaceInsteadOfCombine)
{
    if(deg == 0) return;

    MatrixData n;

    const T r = deg * (M_PI / 180);
    const T s = ::sin(r);
    const T c = ::cos(r);

    n.v[0][0] =  c; n.v[0][1] = s; n.v[0][2] = 0;
    n.v[1][0] = -s; n.v[1][1] = c; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
inline void BasicTransform<T>::shearX(T deg, bool replaceInsteadOfCombine)
{
    if(deg == 0) return;

    MatrixData n;

    const T r = deg * (M_PI / 180);
    const T t = ::sin(r) / ::cos(r);

    n.v[0][0] = 1; n.v[0][1] = t; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
inline void BasicTransform<T>::shearY(T deg, bool replaceInsteadOfCombine)
{
    if(deg == 0) return;

    MatrixData n;

    const T r = deg * (M_PI / 180);
    const T t = ::sin(r) / ::cos(r);

    n.v[0][0] = 1; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] = t; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
inline void BasicTransform<T>::getRaw(T m[3][3]) const
{
    m[0][0] = _mdata.v[0][0]; m[0][1] = _mdata.v[0][1]; m[0][2] = _mdata.v[0][2];
    m[1][0] = _mdata.v[1][0]; m[1][1] = _mdata.v[1][1]; m[1][2] = _mdata.v[1][2];
    m[2][0] = _mdata.v[2][0]; m[2][1] = _mdata.v[2][1]; m[2][2] = _mdata.v[2][2];
}

template <typename T>
inline void BasicTransform<T>::setRaw(const T m[3][3])
{
    // Check if the given raw matrix is an identity matrix
    if( m[0][0] == 1 && m[0][1] == 0 && m[0][2] == 0 &&
        m[1][0] == 0 && m[1][1] == 1 && m[1][2] == 0 &&
        m[2][0] == 0 && m[2][1] == 0 && m[2][2] == 1
    ) {
        this->identity();
        return;
    }

    _mdata.v[0][0] = m[0][0]; _mdata.v[0][1] = m[0][1]; _mdata.v[0][2] = m[0][2];
    _mdata.v[1][0] = m[1][0]; _mdata.v[1][1] = m[1][1]; _mdata.v[1][2] = m[1][2];
    _mdata.v[2][0] = m[2][0]; _mdata.v[2][1] = m[2][1]; _mdata.v[2][2] = m[2][2];
    _isIdentity = false;
}

template <typename T>
inline const BasicTransform<T>& BasicTransform<T>::operator=(const BasicTransform<T>& m)
{
    this->_mdata      = m._mdata;
    this->_isIdentity = m._isIdentity;

    return *this;
}

template <typename T>
inline const BasicTransform<T> BasicTransform<T>::operator*(const BasicTransform<T>& m) const
{
    // Copy this matrix
    BasicTransform r = *this;

    // Check if the given matrix is an identity matrix
    if(m._isIdentity) return r;

    // Normal operation
    r.updateMatrix(m._mdata, false);
    r._isIdentity = false;

    return r;
}

template <typename T>
inline bool BasicTransform<T>::operator==(const BasicTransform<T>& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) == 0; }

template <typename T>
inline bool BasicTransform<T>::operator!=(const BasicTransform<T>& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) != 0; }

template <typename T>
inline void BasicTransform<T>::transformPoint(T& dx, T& dy, T sx, T sy) const
{
    if( _isIdentity || (sx > Painter::MaximumCoordinate && sy > Painter::MaximumCoordinate) ) {
        dx = sx;
        dy = sy;
        return;
    }

    const T tx = _mdata.v[0][0] * sx + _mdata.v[0][1] * sy + _mdata.v[0][2];
    const T ty = _mdata.v[1][0] * sx + _mdata.v[1][1] * sy + _mdata.v[1][2];

    dx = tx;
    dy = ty;
}

template <typename T>
inline void BasicTransform<T>::transformPoint(T& x, T &y) const
{ transformPoint(x, y, x, y); }

template <typename T>
inline void BasicTransform<T>::transformPoints(T* dxy, const T* sxy, size_t pointCount) const
{
    pointCount *= 2;

    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(T));
        return;
    }

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

template <typename T>
inline void BasicTransform<T>::transformPoints(T* xy, size_t pointCount) const
{
    if(_isIdentity) return;

    pointCount *= 2;

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(xy[i], xy[i + 1]);
}

template <typename T>
inline void BasicTransform<T>::transformPoint(PointF& dp, const PointF& sp) const
{
    if( _isIdentity || (sp.x() > Painter::MaximumCoordinate && sp.y() > Painter::MaximumCoordinate) ) {
        dp = sp;
        return;
    }

    T x = sp.x();
    T y = sp.y();

    transformPoint(x, y);

    dp.set(x, y);
}

template <typename T>
inline void BasicTransform<T>::transformPoint(PointF& p) const
{ transformPoint(p, p); }

template <typename T>
inline void BasicTransform<T>::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const
{
    if(_isIdentity) {
        for(size_t i = 0; i < pointCount; ++i) dxy[i] = sxy[i];
        return;
    }

    for(size_t i = 0; i < pointCount; ++i) transformPoint(dxy[i], sxy[i]);
}

template <typename T>
inline void BasicTransform<T>::transformPoints(PointF* xy, size_t pointCount) const
{
    if(_isIdentity) return;

    for(size_t i = 0; i < pointCount; ++i) transformPoint(xy[i]);
}

// ======================================================================================
// ===== Inlined Public Member Functions (Specialization for float) =====================
// ======================================================================================

template <>
inline void BasicTransform<float>::rotate(float deg, bool replaceInsteadOfCombine)
{
    if(deg == 0) return;

    MatrixData n;

    const float r = deg * Gfx::Math::PiDiv180;
    const float s = Gfx::Math::fastSin(r);
    const float c = Gfx::Math::fastCos(r);

    n.v[0][0] =  c; n.v[0][1] = s; n.v[0][2] = 0;
    n.v[1][0] = -s; n.v[1][1] = c; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <>
inline void BasicTransform<float>::shearX(float deg, bool replaceInsteadOfCombine)
{
    if(deg == 0) return;

    MatrixData n;

    const float r = deg * Gfx::Math::PiDiv180;
    const float t = Gfx::Math::fastSin(r) / Gfx::Math::fastCos(r);

    n.v[0][0] = 1; n.v[0][1] = t; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <>
inline void BasicTransform<float>::shearY(float deg, bool replaceInsteadOfCombine)
{
    if(deg == 0) return;

    MatrixData n;

    const float r = deg * Gfx::Math::PiDiv180;
    const float t = Gfx::Math::fastSin(r) / Gfx::Math::fastCos(r);

    n.v[0][0] = 1; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] = t; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

#if defined(PT_GFX_USE_AVX1)

template <>
inline void BasicTransform<float>::transformPoints(float* dxy, const float* sxy, size_t pointCount) const
{
    pointCount *= 2;

    if(_isIdentity) {
        if(dxy != sxy) memcpy(dxy, sxy, pointCount * sizeof(float));
        return;
    }

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    // Load and extend the matrix's rows                                                          // [ RC RC RC RC RC RC RC RC ]
    const __m256 m00 = _mm256_insertf128_ps(_mm256_castps128_ps256(_mdata.r[0]), _mdata.r[0], 1); // [ 00 01 02 03 00 01 02 03 ]
    const __m256 m11 = _mm256_insertf128_ps(_mm256_castps128_ps256(_mdata.r[1]), _mdata.r[1], 1); // [ 13 12 11 10 13 12 11 10 ]

    // Loop through 8 floats at a time
    const size_t pointCount8 = pointCount / 8;

    for(size_t i = 0; i < pointCount8; ++i) {
        // Load 8 floats from the source vector                                              // [ #3 #2 #1 #0 #3 #2 #1 #0 ]
        const __m256 s3210 = _mm256_loadu_ps  (sxy                                        ); // [ Y3 X3 Y2 X2 Y1 X1 Y0 X0 ]
        const __m256 s32   = _mm256_shuffle_ps(s3210, avxOneZeroF, _MM_SHUFFLE(3, 2, 3, 2)); // [ 0  1  Y3 X3 0  1  Y1 X1 ]
        const __m256 s10   = _mm256_shuffle_ps(s3210, avxOneZeroF, _MM_SHUFFLE(1, 0, 1, 0)); // [ 0  1  Y2 X2 0  1  Y0 X0 ]
        // Multiply them to the matrix's rows         // [ RC RC RC RC RC RC RC RC ]
        const __m256 r32_0 = _mm256_mul_ps(m00, s32); // [ 00 01 02 03 00 01 02 03 ]
        const __m256 r32_1 = _mm256_mul_ps(m11, s32); // [ 13 12 11 10 13 12 11 10 ]
        const __m256 r10_0 = _mm256_mul_ps(m00, s10); // [ 00 01 02 03 00 01 02 03 ]
        const __m256 r10_1 = _mm256_mul_ps(m11, s10); // [ 13 12 11 10 13 12 11 10 ]
        // Horizontal add the multiplication results
        const __m256 r32   = _mm256_hadd_ps(r32_0, r32_1);
        const __m256 r10   = _mm256_hadd_ps(r10_0, r10_1);
        const __m256 r3210 = _mm256_hadd_ps(r10  , r32  );
        // Store 8 floats to the destination vector
        const __m256 mask  = _mm256_cmp_ps(s3210, avxMaxCordF, _CMP_GT_OQ);
#if 1
        _mm256_storeu_ps(dxy, _mm256_blendv_ps(
                                  r3210, // Retain result values <= maximum coordinate
                                  s3210, // Retain source values >  maximum coordinate
                                  mask
                              )
                        );
#else
        _mm256_storeu_ps(
            dxy,
            _mm256_or_ps(
                _mm256_and_ps   (mask, s3210), // Retain source values >  maximum coordinate
                _mm256_andnot_ps(mask, r3210)  // Retain result values <= maximum coordinate
            )
        );
#endif
        // Increment the pointers
        sxy += 8;
        dxy += 8;
    }

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    // Process the remaining floats using normal code
    pointCount %= 8;

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

#elif defined(PT_GFX_USE_NEON)

template <>
inline void BasicTransform<float>::transformPoints(float* dxy, const float* sxy, size_t pointCount) const
{
    pointCount *= 2;

    if(_isIdentity) {
        if(dxy != sxy) memcpy(dxy, sxy, pointCount * sizeof(float));
        return;
    }

    // Loop through 4 floats at a time
    const size_t pointCount4 = pointCount / 4;

    for(size_t i = 0; i < pointCount4; ++i) {
        // Load 4 floats from the source vector                                                    // [ --H-- --L-- ]
        const float32x4_t s3210 = vld1q_f32   (sxy                                              ); // [ Y1 X1 Y0 X0 ]
        const float32x4_t s32   = vcombine_f32(vget_high_f32(s3210), vget_high_f32(neonOneZeroF)); // [ 0  1  Y1 X1 ]
        const float32x4_t s10   = vcombine_f32(vget_low_f32 (s3210), vget_low_f32 (neonOneZeroF)); // [ 0  1  Y0 X0 ]
        // Multiply them to the matrix's rows                  // [ RC RC RC RC ]
        const float32x4_t r32_0 = vmulq_f32(_mdata.r[0], s32); // [ 03 02 01 00 ]
        const float32x4_t r32_1 = vmulq_f32(_mdata.r[1], s32); // [ 13 12 11 10 ]
        const float32x4_t r10_0 = vmulq_f32(_mdata.r[0], s10); // [ 03 02 01 00 ]
        const float32x4_t r10_1 = vmulq_f32(_mdata.r[1], s10); // [ 13 12 11 10 ]
        // Horizontal add the multiplication results
        const float32x4_t r32   = vcombine_f32( vpadd_f32( vget_low_f32(r32_0), vget_high_f32(r32_0) ), vpadd_f32( vget_low_f32(r32_1), vget_high_f32(r32_1) ) );
        const float32x4_t r10   = vcombine_f32( vpadd_f32( vget_low_f32(r10_0), vget_high_f32(r10_0) ), vpadd_f32( vget_low_f32(r10_1), vget_high_f32(r10_1) ) );
        const float32x4_t r3210 = vcombine_f32( vpadd_f32( vget_low_f32(r10  ), vget_high_f32(r10  ) ), vpadd_f32( vget_low_f32(r32  ), vget_high_f32(r32  ) ) );
        // Store 4 floats to the destination vector
        const uint32x4_t  mask  = vcgtq_f32(s3210, neonMaxCordF);
        vst1q_f32(
            dxy,
            (float32x4_t) vorrq_s32(
                vandq_s32((int32x4_t) s3210, (int32x4_t) mask), // Retain source values >  maximum coordinate
                vbicq_s32((int32x4_t) r3210, (int32x4_t) mask)  // Retain result values <= maximum coordinate
            )
        );
        /*
        vst1q_f32(
            dxy,
            (float32x4_t) vorrq_s32(
                vandq_s32((int32x4_t) s3210, (int32x4_t) vcgtq_f32(s3210, neonMaxCordF)), // Retain source values >  maximum coordinate
                vandq_s32((int32x4_t) r3210, (int32x4_t) vcleq_f32(s3210, neonMaxCordF))  // Retain result values <= maximum coordinate
            )
        );
        */
        // Increment the pointers
        sxy += 4;
        dxy += 4;
    }

    // Process the remaining floats using normal code
    pointCount %= 4;

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

#endif

template <>
inline void BasicTransform<float>::transformPoints(float* xy, size_t pointCount) const
{ if(!_isIdentity) transformPoints(xy, xy, pointCount); }

template <>
inline void BasicTransform<float>::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const
{
    if(_isIdentity) {
        for(size_t i = 0; i < pointCount; ++i) dxy[i] = sxy[i];
        return;
    }

    float  xy[pointCount * 2];
    float* pxy = xy;

    for(size_t i = 0; i < pointCount; ++i) {
        *pxy++ = sxy[i].x();
        *pxy++ = sxy[i].y();
    }

    transformPoints(xy, xy, pointCount);

    pxy = xy;

    for(size_t i = 0; i < pointCount; ++i) {
        dxy[i].setX( *pxy++ );
        dxy[i].setY( *pxy++ );
    }
}

template <>
inline void BasicTransform<float>::transformPoints(PointF* xy, size_t pointCount) const
{ if(!_isIdentity) transformPoints(xy, xy, pointCount); }


//
// For convenience
//
typedef BasicTransform<float > Transform;  // We cannot use ssize_t
typedef BasicTransform<double> TransformF;


/*
// ### TODO: Extract the translation factor
const TransformT::ValueT tx = v[0][2];
const TransformT::ValueT ty = v[1][2];

// ### TODO: Extract the scaling factor
const TransformT::ValueT sx = ::sqrt(v[0][0] * v[0][0] + v[0][1] * v[0][1]);
const TransformT::ValueT sy = ::sqrt(v[1][0] * v[1][0] + v[1][1] * v[1][1]);

// ### TODO: Extract the rotation factor
const TransformT::ValueT r1 = ::atan2(-v[0][1], v[0][0]);
const TransformT::ValueT r2 = ::atan2( v[1][0], v[1][1]);
const TransformT::ValueT r  = (r1 + r2) * 0.5f;
*/


} // namespace
} // namespace

#endif
