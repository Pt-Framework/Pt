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

#include <cstring>
#include <vector>

#include <Pt/Gfx/Math.h>
#include <Pt/Gfx/Painter.h>

#include <Pt/Gfx/SIMDConfig.h>

#undef PT_GFX_USE_AVX2
#undef PT_GFX_USE_AVX1

#undef PT_GFX_USE_SSSE3
#undef PT_GFX_USE_SSE3
#undef PT_GFX_USE_SSE2
#undef PT_GFX_USE_SSE1

#undef PT_GFX_USE_NEON


namespace Pt{
namespace Gfx{


#if defined(PT_GFX_USE_AVX1)

// AVX constants
static const __m256 avxOneZero = _mm256_set_ps(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
static const __m256 avxMaxCord = _mm256_set1_ps(Painter::MaximumCoordinateF);

#endif

#if defined(PT_GFX_USE_NEON)

// NEON constants
static const float32x4_t neonOneZero = NEON_SET_FLT32X4(0.0f, 1.0f, 0.0f, 1.0f);
static const float32x4_t neonMaxCord = vdupq_n_f32(Painter::MaximumCoordinateF);

#endif


/** @brief 2D affine matrix for 2D path transformation.
  */
template <typename T>
class PT_GFX_API BasicAffineMatrix2D {
    public:
        enum MatrixUpdateMode {
            Replace,         //! @brief M' = N
            MultiplyOnLeft,  //! @brief M' = N * M
            MultiplyOnRight  //! @brief M' = M * N
        };

    public:
        inline BasicAffineMatrix2D();
        inline BasicAffineMatrix2D(const BasicAffineMatrix2D& m);

        inline ~BasicAffineMatrix2D();

        inline void clear();

        inline void identity();

        inline void translate(T x, T y, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void scaleAboutOrigin(T x, T y, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void rotateAboutOrigin(T deg, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void shearXDirection(T deg, MatrixUpdateMode mode = MultiplyOnLeft);
        inline void shearYDirection(T deg, MatrixUpdateMode mode = MultiplyOnLeft);

        inline void reflectAboutOrigin(MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutXAxis(MatrixUpdateMode mode = MultiplyOnLeft);
        inline void reflectAboutYAxis(MatrixUpdateMode mode = MultiplyOnLeft);

        inline void getRaw(T m[3][3]) const;
        inline void updateUsingRaw(const T m[3][3], MatrixUpdateMode mode = MultiplyOnLeft);

        inline const BasicAffineMatrix2D& operator=(const BasicAffineMatrix2D& m);
        inline const BasicAffineMatrix2D& operator*(const BasicAffineMatrix2D& m);

        inline bool operator==(const BasicAffineMatrix2D& m) const;
        inline bool operator!=(const BasicAffineMatrix2D& m) const;

        inline void push();
        inline bool pop();

        inline void transformPoint(T& dx, T& dy, T sx, T sy) const;
        inline void transformPoint(T& x, T &y) const;

        inline void transformPoint(PointF& dp, const PointF& sp) const;
        inline void transformPoint(PointF& p) const;

        inline void transformPoints(T* dxy, const T* sxy, size_t pointCount) const;
        inline void transformPoints(T* xy, size_t pointCount) const;

        inline void transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const;
        inline void transformPoints(PointF* xy, size_t pointCount) const;

    private:
        // Matrix stack
#if defined(PT_GFX_USE_AVX1)
        union MatrixData {
            T      v[4][4];
            __m128 r[4];
        };
#elif defined(PT_GFX_USE_NEON)
        union MatrixData {
            T           v[4][4];
            float32x4_t r[4];
        };
#else
        struct MatrixData {
            T v[3][3];
        };
#endif

        // Matrix stack data
        struct StackData {
            MatrixData mdata;
            bool       isIdentity;

            inline StackData(const MatrixData& mdata_, bool isIdentity_)
            : mdata(mdata_), isIdentity(isIdentity_)
            {}
        };

    private:
        inline void updateMatrix(const MatrixData& n, MatrixUpdateMode mode);

    private:
        MatrixData             _mdata;
        bool                   _isIdentity;

        std::vector<StackData> _stack;
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

template <typename T>
void BasicAffineMatrix2D<T>::updateMatrix(const MatrixData& n, MatrixUpdateMode mode)
{
    // Check if the current matrix is an identity matrix or the mode is "Replace"
    if(_isIdentity || mode == Replace) {
        _mdata = n;
        return;
    }

    // Multiply based on the mode
    const MatrixData* l;
    const MatrixData* r;
          MatrixData  o;

    if(mode == MultiplyOnLeft) {
        l = &n;
        r = &_mdata;
    }
    else { // MultiplyOnRight
        l = &_mdata;
        r = &n;
    }

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
// ===== Inlined Private Member Functions (Specialization for float) ====================
// ======================================================================================

#if defined(PT_GFX_USE_AVX1)

template <>
void BasicAffineMatrix2D<float>::updateMatrix(const MatrixData& n, MatrixUpdateMode mode)
{
    // Check if the current matrix is an identity matrix or the mode is "Replace"
    if(_isIdentity || mode == Replace) {
        _mdata = n;
        return;
    }

    // Multiply based on the mode
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
}

#elif defined(PT_GFX_USE_NEON)

// ### The NEON version is actually slower than the plain Arm version ###

template <>
void BasicAffineMatrix2D<float>::updateMatrix(const MatrixData& n, MatrixUpdateMode mode)
{
    // Check if the current matrix is an identity matrix or the mode is "Replace"
    if(_isIdentity || mode == Replace) {
        _mdata = n;
        return;
    }

    // Multiply based on the mode
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

    float32x4_t out0x =                   vmulq_f32(vld1q_dup_f32(&l->v[0][0]), r->r[0])  ;
                out0x = vaddq_f32( out0x, vmulq_f32(vld1q_dup_f32(&l->v[0][1]), r->r[1]) );
                out0x = vaddq_f32( out0x, vmulq_f32(vld1q_dup_f32(&l->v[0][2]), r->r[2]) );


    float32x4_t out1x =                   vmulq_f32(vld1q_dup_f32(&l->v[1][0]), r->r[0])  ;
                out1x = vaddq_f32( out1x, vmulq_f32(vld1q_dup_f32(&l->v[1][1]), r->r[1]) );
                out1x = vaddq_f32( out1x, vmulq_f32(vld1q_dup_f32(&l->v[1][2]), r->r[2]) );

    float32x4_t out2x =                   vmulq_f32(vld1q_dup_f32(&l->v[2][0]), r->r[0])  ;
                out2x = vaddq_f32( out2x, vmulq_f32(vld1q_dup_f32(&l->v[2][1]), r->r[1]) );
                out2x = vaddq_f32( out2x, vmulq_f32(vld1q_dup_f32(&l->v[2][2]), r->r[2]) );

    vst1q_f32(_mdata.v[0], out0x);
    vst1q_f32(_mdata.v[1], out1x);
    vst1q_f32(_mdata.v[2], out2x);
}

#endif


// ======================================================================================
// ===== Inlined Public Member Functions ================================================
// ======================================================================================

template <typename T>
BasicAffineMatrix2D<T>::BasicAffineMatrix2D()
{
    identity();

#if defined(PT_GFX_USE_AVX1) || defined(PT_GFX_USE_NEON)
    _mdata.v[0][3] = 0; _mdata.v[1][3] = 0; _mdata.v[2][3] = 0;
    _mdata.v[3][0] = 0; _mdata.v[3][1] = 0; _mdata.v[3][2] = 0; _mdata.v[3][3] = 0;
#endif
}

template <typename T>
BasicAffineMatrix2D<T>::BasicAffineMatrix2D(const BasicAffineMatrix2D<T>& m)
{ *this = m; }

template <typename T>
BasicAffineMatrix2D<T>::~BasicAffineMatrix2D()
{}

template <typename T>
void BasicAffineMatrix2D<T>::clear()
{
    identity();

    _stack.clear();
}

template <typename T>
void BasicAffineMatrix2D<T>::identity()
{
    _mdata.v[0][0] = 1; _mdata.v[0][1] = 0; _mdata.v[0][2] = 0;
    _mdata.v[1][0] = 0; _mdata.v[1][1] = 1; _mdata.v[1][2] = 0;
    _mdata.v[2][0] = 0; _mdata.v[2][1] = 0; _mdata.v[2][2] = 1;

    _isIdentity = true;
}

template <typename T>
void BasicAffineMatrix2D<T>::translate(T x, T y, MatrixUpdateMode mode)
{
    if(x == 0 && y == 0) return;

    MatrixData n;

    n.v[0][0] = 1; n.v[0][1] = 0; n.v[0][2] = x;
    n.v[1][0] = 0; n.v[1][1] = 1; n.v[1][2] = y;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::scaleAboutOrigin(T x, T y, MatrixUpdateMode mode)
{
    if(x == 1 && y == 1) return;

    MatrixData n;

    n.v[0][0] = x; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = y; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::rotateAboutOrigin(T deg, MatrixUpdateMode mode)
{
    if(deg == 0) return;

    MatrixData n;

    const T r = deg * (M_PI / 180);
    const T s = ::sin(r);
    const T c = ::cos(r);

    n.v[0][0] =  c; n.v[0][1] = s; n.v[0][2] = 0;
    n.v[1][0] = -s; n.v[1][1] = c; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::shearXDirection(T deg, MatrixUpdateMode mode)
{
    if(deg == 0) return;

    MatrixData n;

    const T r = deg * (M_PI / 180);
    const T t = ::sin(r) / ::cos(r);

    n.v[0][0] = 1; n.v[0][1] = t; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::shearYDirection(T deg, MatrixUpdateMode mode)
{
    if(deg == 0) return;

    MatrixData n;

    const T r = deg * (M_PI / 180);
    const T t = ::sin(r) / ::cos(r);

    n.v[0][0] = 1; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] = t; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::reflectAboutOrigin(MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = -1; n.v[0][1] =  0; n.v[0][2] = 0;
    n.v[1][0] =  0; n.v[1][1] = -1; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] =  0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::reflectAboutXAxis(MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = 1; n.v[0][1] =  0; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = -1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] =  0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::reflectAboutYAxis(MatrixUpdateMode mode)
{
    MatrixData n;

    n.v[0][0] = -1; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] =  0; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::getRaw(T m[3][3]) const
{
    m[0][0] = _mdata.v[0][0]; m[0][1] = _mdata.v[0][1]; m[0][2] = _mdata.v[0][2];
    m[1][0] = _mdata.v[1][0]; m[1][1] = _mdata.v[1][1]; m[1][2] = _mdata.v[1][2];
    m[2][0] = _mdata.v[2][0]; m[2][1] = _mdata.v[2][1]; m[2][2] = _mdata.v[2][2];
}

template <typename T>
void BasicAffineMatrix2D<T>::updateUsingRaw(const T m[3][3], MatrixUpdateMode mode)
{
    // Check if the given raw matrix is an identity matrix
    if( m[0][0] == 1 && m[0][1] == 0 && m[0][2] == 0 &&
        m[1][0] == 0 && m[1][1] == 1 && m[1][2] == 0 &&
        m[2][0] == 0 && m[2][1] == 0 && m[2][2] == 1
    ) {
        if(mode == Replace) this->identity();
        return;
    }

    // Normal operation
    MatrixData n;

    n.v[0][0] = m[0][0]; n.v[0][1] = m[0][1]; n.v[0][2] = m[0][2];
    n.v[1][0] = m[1][0]; n.v[1][1] = m[1][1]; n.v[1][2] = m[1][2];
    n.v[2][0] = m[2][0]; n.v[2][1] = m[2][1]; n.v[2][2] = m[2][2];

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <typename T>
const BasicAffineMatrix2D<T>& BasicAffineMatrix2D<T>::operator=(const BasicAffineMatrix2D<T>& m)
{
    this->_mdata      = m._mdata;
    this->_isIdentity = m._isIdentity;

    this->_stack      = m._stack;

    return *this;
}

template <typename T>
const BasicAffineMatrix2D<T>& BasicAffineMatrix2D<T>::operator*(const BasicAffineMatrix2D<T>& m)
{
    // Check if the given matrix is an identity matrix
    if(m._isIdentity) return *this;

    // Normal operation
    updateMatrix(m._mdata, MultiplyOnRight);
    _isIdentity = false;

    return *this;
}

template <typename T>
bool BasicAffineMatrix2D<T>::operator==(const BasicAffineMatrix2D<T>& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) == 0; }

template <typename T>
bool BasicAffineMatrix2D<T>::operator!=(const BasicAffineMatrix2D<T>& m) const
{ return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) != 0; }

template <typename T>
void BasicAffineMatrix2D<T>::push()
{ _stack.push_back( StackData(_mdata, _isIdentity) ); }

template <typename T>
bool BasicAffineMatrix2D<T>::pop()
{
    if(_stack.empty()) return false;

    _mdata      = _stack.back().mdata;
    _isIdentity = _stack.back().isIdentity;

    _stack.pop_back();

    return true;
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(T& dx, T& dy, T sx, T sy) const
{
    if( _isIdentity || (sx > Painter::MaximumCoordinateF && sy > Painter::MaximumCoordinateF) ) {
        dx = sx;
        dy = sy;
        return;
    }

    dx = _mdata.v[0][0] * sx + _mdata.v[0][1] * sy + _mdata.v[0][2];
    dy = _mdata.v[1][0] * sx + _mdata.v[1][1] * sy + _mdata.v[1][2];
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(T& x, T &y) const
{
    if( _isIdentity || (x > Painter::MaximumCoordinateF && y > Painter::MaximumCoordinateF) ) return;

    const T tx = _mdata.v[0][0] * x + _mdata.v[0][1] * y + _mdata.v[0][2];
    const T ty = _mdata.v[1][0] * x + _mdata.v[1][1] * y + _mdata.v[1][2];

    x = tx;
    y = ty;
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(PointF& dp, const PointF& sp) const
{
    if( _isIdentity || (sp.x() > Painter::MaximumCoordinateF && sp.y() > Painter::MaximumCoordinateF) ) {
        dp = sp;
        return;
    }

    dp.set(
        _mdata.v[0][0] * sp.x() + _mdata.v[0][1] * sp.y() + _mdata.v[0][2],
        _mdata.v[1][0] * sp.x() + _mdata.v[1][1] * sp.y() + _mdata.v[1][2]
    );
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(PointF& p) const
{
    if( _isIdentity || (p.x() > Painter::MaximumCoordinateF && p.y() > Painter::MaximumCoordinateF) ) return;

    p.set(
        _mdata.v[0][0] * p.x() + _mdata.v[0][1] * p.y() + _mdata.v[0][2],
        _mdata.v[1][0] * p.x() + _mdata.v[1][1] * p.y() + _mdata.v[1][2]
    );
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoints(T* dxy, const T* sxy, size_t pointCount) const
{
    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(T));
        return;
    }

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoints(T* xy, size_t pointCount) const
{
    if(_isIdentity) return;

    transformPoints(xy, xy, pointCount);
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const
{
    if(_isIdentity) {
        for(size_t i = 0; i < pointCount; ++i) dxy[i] = sxy[i];
        return;
    }

    for(size_t i = 0; i < pointCount; ++i) transformPoint(dxy[i], sxy[i]);
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoints(PointF* xy, size_t pointCount) const
{
    if(_isIdentity) return;

    transformPoints(xy, xy, pointCount);
}

// ======================================================================================
// ===== Inlined Public Member Functions (Specialization for float) =====================
// ======================================================================================

template <>
void BasicAffineMatrix2D<float>::rotateAboutOrigin(float deg, MatrixUpdateMode mode)
{
    if(deg == 0) return;

    MatrixData n;

    const float r = deg * Gfx::Math::PiDiv180;
    const float s = Gfx::Math::fastSin(r);
    const float c = Gfx::Math::fastCos(r);

    n.v[0][0] =  c; n.v[0][1] = s; n.v[0][2] = 0;
    n.v[1][0] = -s; n.v[1][1] = c; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <>
void BasicAffineMatrix2D<float>::shearXDirection(float deg, MatrixUpdateMode mode)
{
    if(deg == 0) return;

    MatrixData n;

    const float r = deg * Gfx::Math::PiDiv180;
    const float t = Gfx::Math::fastSin(r) / Gfx::Math::fastCos(r);

    n.v[0][0] = 1; n.v[0][1] = t; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

template <>
void BasicAffineMatrix2D<float>::shearYDirection(float deg, MatrixUpdateMode mode)
{
    if(deg == 0) return;

    MatrixData n;

    const float r = deg * Gfx::Math::PiDiv180;
    const float t = Gfx::Math::fastSin(r) / Gfx::Math::fastCos(r);

    n.v[0][0] = 1; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] = t; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, mode);
    _isIdentity = false;
}

#if defined(PT_GFX_USE_AVX1)

template <>
void BasicAffineMatrix2D<float>::transformPoints(float* dxy, const float* sxy, size_t pointCount) const
{
    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(float));
        return;
    }

    // Load the matrix's rows
    const __m128 m0 = _mm_loadu_ps(_mdata.v[0]); // [ 00 01 02 03 ]
    const __m128 m1 = _mm_loadu_ps(_mdata.v[1]); // [ 10 11 12 13 ]

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    // Extend the matrix's rows
    const __m256 m00 = _mm256_insertf128_ps(_mm256_castps128_ps256(m0), m0, 1); // [ 00 01 02 03 00 01 02 03 ]
    const __m256 m11 = _mm256_insertf128_ps(_mm256_castps128_ps256(m1), m1, 1); // [ 10 11 12 13 10 11 12 13 ]

    // Loop through 8 floats at a time
    const size_t  pointCount8 = pointCount / 8;

    for(size_t i = 0; i < pointCount8; ++i) {
        // Load 8 floats from the source vector
        const __m256 s3210 = _mm256_loadu_ps  (sxy                                       ); // [ X0 Y0 X1 Y1 X2 Y2 X3 Y3 ]
        const __m256 s32   = _mm256_shuffle_ps(s3210, avxOneZero, _MM_SHUFFLE(3, 2, 3, 2)); // [ X0 Y0 1  0  X2 Y2 1  0  ]
        const __m256 s10   = _mm256_shuffle_ps(s3210, avxOneZero, _MM_SHUFFLE(1, 0, 1, 0)); // [ X1 Y1 1  0  X3 Y3 1  0  ]
        // Multiply them to the matrix's rows
        const __m256 r32_0 = _mm256_mul_ps(m00, s32);
        const __m256 r32_1 = _mm256_mul_ps(m11, s32);
        const __m256 r10_0 = _mm256_mul_ps(m00, s10);
        const __m256 r10_1 = _mm256_mul_ps(m11, s10);
        // Horizontal add the multiplication results
        const __m256 r32   = _mm256_hadd_ps(r32_0, r32_1);
        const __m256 r10   = _mm256_hadd_ps(r10_0, r10_1);
        const __m256 r3210 = _mm256_hadd_ps(r10  , r32  );
        // Store 8 floats to the destination vector
        _mm256_storeu_ps(
            dxy,
            _mm256_or_ps(
                _mm256_and_ps(s3210, _mm256_cmp_ps(s3210, avxMaxCord, _CMP_GT_OQ)), // Retain source values >  maximum coordinate
                _mm256_and_ps(r3210, _mm256_cmp_ps(s3210, avxMaxCord, _CMP_LE_OQ))  // Retain result values <= maximum coordinate
            )
        );
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
void BasicAffineMatrix2D<float>::transformPoints(float* dxy, const float* sxy, size_t pointCount) const
{
    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(float));
        return;
    }

    // Load the matrix's rows
    const float32x4_t m0 = vld1q_f32(_mdata.v[0]); // [ 00 01 02 03 ]
    const float32x4_t m1 = vld1q_f32(_mdata.v[1]); // [ 10 11 12 13 ]

    // Loop through 4 floats at a time
    const size_t  pointCount4 = pointCount / 4;

    for(size_t i = 0; i < pointCount4; ++i) {
        // Load 4 floats from the source vector
        const float32x4_t s3210 = vld1q_f32   (sxy                                             ); // [ X0 Y0 X1 Y1 ]
        const float32x4_t s32   = vcombine_f32(vget_high_f32(s3210), vget_high_f32(neonOneZero)); // [ X0 Y0 1  0  ]
        const float32x4_t s10   = vcombine_f32(vget_low_f32 (s3210), vget_low_f32 (neonOneZero)); // [ X1 Y1 1  0  ]
        // Multiply them to the matrix's rows
        const float32x4_t r32_0 = vmulq_f32(m0, s32);
        const float32x4_t r32_1 = vmulq_f32(m1, s32);
        const float32x4_t r10_0 = vmulq_f32(m0, s10);
        const float32x4_t r10_1 = vmulq_f32(m1, s10);
        // Horizontal add the multiplication results
        const float32x4_t r32   = vcombine_f32( vpadd_f32( vget_low_f32(r32_0), vget_high_f32(r32_0) ), vpadd_f32( vget_low_f32(r32_1), vget_high_f32(r32_1) ) );
        const float32x4_t r10   = vcombine_f32( vpadd_f32( vget_low_f32(r10_0), vget_high_f32(r10_0) ), vpadd_f32( vget_low_f32(r10_1), vget_high_f32(r10_1) ) );
        const float32x4_t r3210 = vcombine_f32( vpadd_f32( vget_low_f32(r10  ), vget_high_f32(r10  ) ), vpadd_f32( vget_low_f32(r32  ), vget_high_f32(r32  ) ) );
        // Store 4 floats to the destination vector
        vst1q_f32(
            dxy,
            (float32x4_t) vorrq_s32(
                vandq_s32((int32x4_t) s3210, (int32x4_t) vcgtq_f32(s3210, neonMaxCord)), // Retain source values >  maximum coordinate
                vandq_s32((int32x4_t) r3210, (int32x4_t) vcleq_f32(s3210, neonMaxCord))  // Retain result values <= maximum coordinate
            )
        );
        // Increment the pointers
        sxy += 4;
        dxy += 4;
    }

    // Process the remaining floats using normal code
    pointCount %= 4;

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

#endif

#if defined(PT_GFX_USE_AVX1) || defined(PT_GFX_USE_NEON)

template <>
void BasicAffineMatrix2D<float>::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const
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

    transformPoints(xy, xy, pointCount * 2);

    pxy = xy;

    for(size_t i = 0; i < pointCount; ++i) {
        dxy[i].setX( *pxy++ );
        dxy[i].setY( *pxy++ );
    }
}
#endif


//
// For convenience
//
typedef BasicAffineMatrix2D<float> AffineMatrix2D;


} // namespace
} // namespace

#endif
