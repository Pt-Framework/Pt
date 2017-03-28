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


namespace Pt{
namespace Gfx{


#if defined(PT_GFX_USE_AVX1)

// AVX constants
static const __m256  avxOneZeroF = _mm256_set_ps(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
static const __m256  avxMaxCordF = _mm256_set1_ps(Painter::MaximumCoordinateF);

static const __m256d avxOneZeroD = _mm256_set_pd(0.0, 1.0, 0.0, 1.0);
static const __m256d avxMaxCordD = _mm256_set1_pd(Painter::MaximumCoordinateF);

#endif

#if defined(PT_GFX_USE_NEON)

// NEON constants
static const float32x4_t neonOneZeroF = NEON_SET_FLT32X4(0.0f, 1.0f, 0.0f, 1.0f);
static const float32x4_t neonMaxCordF = vdupq_n_f32(Painter::MaximumCoordinateF);

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


/** @brief 2D affine matrix for 2D path transformation.
  */
template <typename T>
class PT_GFX_API BasicAffineMatrix2D {
    public:
        inline BasicAffineMatrix2D();
        inline BasicAffineMatrix2D(const BasicAffineMatrix2D& m);

        inline ~BasicAffineMatrix2D();

        inline void clear();

        inline void identity();

        inline void translate(T x, T y, bool replaceInsteadOfCombine = false);

        inline void scale(T x, T y, bool replaceInsteadOfCombine = false);

        inline void rotate(T deg, bool replaceInsteadOfCombine = false);

        inline void shearX(T deg, bool replaceInsteadOfCombine = false);
        inline void shearY(T deg, bool replaceInsteadOfCombine = false);

        inline void reflectXY(bool replaceInsteadOfCombine = false);
        inline void reflectX(bool replaceInsteadOfCombine = false);
        inline void reflectY(bool replaceInsteadOfCombine = false);

        inline void getRaw(T m[3][3]) const;
        inline void setRaw(const T m[3][3]);

        inline const BasicAffineMatrix2D& operator=(const BasicAffineMatrix2D& m);
        inline const BasicAffineMatrix2D& operator*(const BasicAffineMatrix2D& m);

        inline bool operator==(const BasicAffineMatrix2D& m) const;
        inline bool operator!=(const BasicAffineMatrix2D& m) const;

        inline void push();
        inline bool pop();

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

        // Matrix stack data
        struct StackData {
            MatrixData mdata;
            bool       isIdentity;

            inline StackData(const MatrixData& mdata_, bool isIdentity_)
            : mdata(mdata_), isIdentity(isIdentity_)
            {}
        };

    private:
        inline void updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine);

    private:
        MatrixData             _mdata;
        bool                   _isIdentity;

        std::vector<StackData> _stack;
};


// ======================================================================================
// ===== Inlined Private Member Functions ===============================================
// ======================================================================================

template <typename T>
void BasicAffineMatrix2D<T>::updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine)
{
    // Check if we need to simply replace the matrix
    if(_isIdentity || replaceInsteadOfCombine) {
        _mdata = n;
        return;
    }

    // Multiply using the mode M' = M * N
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
// ===== Inlined Private Member Functions (Specialization for float) ====================
// ======================================================================================

#if defined(PT_GFX_USE_AVX1)

template <>
void BasicAffineMatrix2D<float>::updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine)
{
    // Check if we need to simply replace the matrix
    if(_isIdentity || replaceInsteadOfCombine) {
        _mdata = n;
        return;
    }

    // Multiply using the mode M' = M * N
    const MatrixData* l = &n;
    const MatrixData* r = &_mdata;

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

#if defined(PT_GFX_USE_FMA3)

    __m128 out0x = _mm_mul_ps  (_mm_broadcast_ss(&l->v[0][0]), r->r[0]       );
           out0x = _mm_fmadd_ps(_mm_broadcast_ss(&l->v[0][1]), r->r[1], out0x);
           out0x = _mm_fmadd_ps(_mm_broadcast_ss(&l->v[0][2]), r->r[2], out0x);

    __m128 out1x = _mm_mul_ps  (_mm_broadcast_ss(&l->v[1][0]), r->r[0]       );
           out1x = _mm_fmadd_ps(_mm_broadcast_ss(&l->v[1][1]), r->r[1], out1x);
           out1x = _mm_fmadd_ps(_mm_broadcast_ss(&l->v[1][2]), r->r[2], out1x);

    __m128 out2x = _mm_mul_ps  (_mm_broadcast_ss(&l->v[2][0]), r->r[0]       );
           out2x = _mm_fmadd_ps(_mm_broadcast_ss(&l->v[2][1]), r->r[1], out2x);
           out2x = _mm_fmadd_ps(_mm_broadcast_ss(&l->v[2][2]), r->r[2], out2x);

#else

    __m128 out0x =                    _mm_mul_ps(_mm_broadcast_ss(&l->v[0][0]), r->r[0])  ;
           out0x = _mm_add_ps( out0x, _mm_mul_ps(_mm_broadcast_ss(&l->v[0][1]), r->r[1]) );
           out0x = _mm_add_ps( out0x, _mm_mul_ps(_mm_broadcast_ss(&l->v[0][2]), r->r[2]) );

    __m128 out1x =                    _mm_mul_ps(_mm_broadcast_ss(&l->v[1][0]), r->r[0])  ;
           out1x = _mm_add_ps( out1x, _mm_mul_ps(_mm_broadcast_ss(&l->v[1][1]), r->r[1]) );
           out1x = _mm_add_ps( out1x, _mm_mul_ps(_mm_broadcast_ss(&l->v[1][2]), r->r[2]) );

    __m128 out2x =                    _mm_mul_ps(_mm_broadcast_ss(&l->v[2][0]), r->r[0])  ;
           out2x = _mm_add_ps( out2x, _mm_mul_ps(_mm_broadcast_ss(&l->v[2][1]), r->r[1]) );
           out2x = _mm_add_ps( out2x, _mm_mul_ps(_mm_broadcast_ss(&l->v[2][2]), r->r[2]) );
#endif

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    _mm_storeu_ps(_mdata.v[0], out0x);
    _mm_storeu_ps(_mdata.v[1], out1x);
    _mm_storeu_ps(_mdata.v[2], out2x);
}

/*
#elif defined(PT_GFX_USE_NEON)

// ### This NEON code is actually slower than the plain Arm code  ###

template <>
void BasicAffineMatrix2D<float>::updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine)
{
    // Check if we need to simply replace the matrix
    if(_isIdentity || replaceInsteadOfCombine) {
        _mdata = n;
        return;
    }

    // Multiply using the mode M' = M * N
    const MatrixData* l = &n;
    const MatrixData* r = &_mdata;

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
*/

#endif

// ======================================================================================
// ===== Inlined Private Member Functions (Specialization for double) ===================
// ======================================================================================

#if defined(PT_GFX_USE_AVX1)

template <>
void BasicAffineMatrix2D<double>::updateMatrix(const MatrixData& n, bool replaceInsteadOfCombine)
{
    // Check if we need to simply replace the matrix
    if(_isIdentity || replaceInsteadOfCombine) {
        _mdata = n;
        return;
    }

    // Multiply using the mode M' = M * N
    const MatrixData* l = &n;
    const MatrixData* r = &_mdata;

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

#if defined(PT_GFX_USE_FMA3)

    __m256d out0x = _mm256_mul_pd  (_mm256_broadcast_sd(&l->v[0][0]), r->r[0]       );
            out0x = _mm256_fmadd_pd(_mm256_broadcast_sd(&l->v[0][1]), r->r[1], out0x);
            out0x = _mm256_fmadd_pd(_mm256_broadcast_sd(&l->v[0][2]), r->r[2], out0x);

    __m256d out1x = _mm256_mul_pd  (_mm256_broadcast_sd(&l->v[1][0]), r->r[0]       );
            out1x = _mm256_fmadd_pd(_mm256_broadcast_sd(&l->v[1][1]), r->r[1], out1x);
            out1x = _mm256_fmadd_pd(_mm256_broadcast_sd(&l->v[1][2]), r->r[2], out1x);

    __m256d out2x = _mm256_mul_pd  (_mm256_broadcast_sd(&l->v[2][0]), r->r[0]       );
            out2x = _mm256_fmadd_pd(_mm256_broadcast_sd(&l->v[2][1]), r->r[1], out2x);
            out2x = _mm256_fmadd_pd(_mm256_broadcast_sd(&l->v[2][2]), r->r[2], out2x);

#else

    __m256d out0x =                       _mm256_mul_pd(_mm256_broadcast_sd(&l->v[0][0]), r->r[0])  ;
            out0x = _mm256_add_pd( out0x, _mm256_mul_pd(_mm256_broadcast_sd(&l->v[0][1]), r->r[1]) );
            out0x = _mm256_add_pd( out0x, _mm256_mul_pd(_mm256_broadcast_sd(&l->v[0][2]), r->r[2]) );

    __m256d out1x =                       _mm256_mul_pd(_mm256_broadcast_sd(&l->v[1][0]), r->r[0])  ;
            out1x = _mm256_add_pd( out1x, _mm256_mul_pd(_mm256_broadcast_sd(&l->v[1][1]), r->r[1]) );
            out1x = _mm256_add_pd( out1x, _mm256_mul_pd(_mm256_broadcast_sd(&l->v[1][2]), r->r[2]) );

    __m256d out2x =                       _mm256_mul_pd(_mm256_broadcast_sd(&l->v[2][0]), r->r[0])  ;
            out2x = _mm256_add_pd( out2x, _mm256_mul_pd(_mm256_broadcast_sd(&l->v[2][1]), r->r[1]) );
            out2x = _mm256_add_pd( out2x, _mm256_mul_pd(_mm256_broadcast_sd(&l->v[2][2]), r->r[2]) );

#endif

    _mm256_storeu_pd(_mdata.v[0], out0x);
    _mm256_storeu_pd(_mdata.v[1], out1x);
    _mm256_storeu_pd(_mdata.v[2], out2x);

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code
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
void BasicAffineMatrix2D<T>::translate(T x, T y, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<T>::scale(T x, T y, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<T>::rotate(T deg, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<T>::shearX(T deg, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<T>::shearY(T deg, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<T>::reflectXY(bool replaceInsteadOfCombine)
{
    MatrixData n;

    n.v[0][0] = -1; n.v[0][1] =  0; n.v[0][2] = 0;
    n.v[1][0] =  0; n.v[1][1] = -1; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] =  0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::reflectX(bool replaceInsteadOfCombine)
{
    MatrixData n;

    n.v[0][0] = 1; n.v[0][1] =  0; n.v[0][2] = 0;
    n.v[1][0] = 0; n.v[1][1] = -1; n.v[1][2] = 0;
    n.v[2][0] = 0; n.v[2][1] =  0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
    _isIdentity = false;
}

template <typename T>
void BasicAffineMatrix2D<T>::reflectY(bool replaceInsteadOfCombine)
{
    MatrixData n;

    n.v[0][0] = -1; n.v[0][1] = 0; n.v[0][2] = 0;
    n.v[1][0] =  0; n.v[1][1] = 1; n.v[1][2] = 0;
    n.v[2][0] =  0; n.v[2][1] = 0; n.v[2][2] = 1;

    updateMatrix(n, replaceInsteadOfCombine);
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
void BasicAffineMatrix2D<T>::setRaw(const T m[3][3])
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
    updateMatrix(m._mdata, false);
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

    const T tx = _mdata.v[0][0] * sx + _mdata.v[0][1] * sy + _mdata.v[0][2];
    const T ty = _mdata.v[1][0] * sx + _mdata.v[1][1] * sy + _mdata.v[1][2];

    dx = tx;
    dy = ty;
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(T& x, T &y) const
{ transformPoint(x, y, x, y); }

template <typename T>
void BasicAffineMatrix2D<T>::transformPoints(T* dxy, const T* sxy, size_t pointCount) const
{
    pointCount *= 2;

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

    pointCount *= 2;

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(xy[i], xy[i + 1]);
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(PointF& dp, const PointF& sp) const
{
    if( _isIdentity || (sp.x() > Painter::MaximumCoordinateF && sp.y() > Painter::MaximumCoordinateF) ) {
        dp = sp;
        return;
    }

    T x = sp.x();
    T y = sp.y();

    transformPoint(x, y);

    dp.set(x, y);
}

template <typename T>
void BasicAffineMatrix2D<T>::transformPoint(PointF& p) const
{ transformPoint(p, p); }

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

    for(size_t i = 0; i < pointCount; ++i) transformPoint(xy[i]);
}

// ======================================================================================
// ===== Inlined Public Member Functions (Specialization for float) =====================
// ======================================================================================

template <>
void BasicAffineMatrix2D<float>::rotate(float deg, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<float>::shearX(float deg, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<float>::shearY(float deg, bool replaceInsteadOfCombine)
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
void BasicAffineMatrix2D<float>::transformPoints(float* dxy, const float* sxy, size_t pointCount) const
{
    pointCount *= 2;

    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(float));
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
        _mm256_storeu_ps(
            dxy,
            _mm256_or_ps(
                _mm256_and_ps(s3210, _mm256_cmp_ps(s3210, avxMaxCordF, _CMP_GT_OQ)), // Retain source values >  maximum coordinate
                _mm256_and_ps(r3210, _mm256_cmp_ps(s3210, avxMaxCordF, _CMP_LE_OQ))  // Retain result values <= maximum coordinate
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
    pointCount *= 2;

    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(float));
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
        vst1q_f32(
            dxy,
            (float32x4_t) vorrq_s32(
                vandq_s32((int32x4_t) s3210, (int32x4_t) vcgtq_f32(s3210, neonMaxCordF)), // Retain source values >  maximum coordinate
                vandq_s32((int32x4_t) r3210, (int32x4_t) vcleq_f32(s3210, neonMaxCordF))  // Retain result values <= maximum coordinate
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

template <>
void BasicAffineMatrix2D<float>::transformPoints(float* xy, size_t pointCount) const
{ if(!_isIdentity) transformPoints(xy, xy, pointCount); }

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

    transformPoints(xy, xy, pointCount);

    pxy = xy;

    for(size_t i = 0; i < pointCount; ++i) {
        dxy[i].setX( *pxy++ );
        dxy[i].setY( *pxy++ );
    }
}

template <>
void BasicAffineMatrix2D<float>::transformPoints(PointF* xy, size_t pointCount) const
{ if(!_isIdentity) transformPoints(xy, xy, pointCount); }

// ======================================================================================
// ===== Inlined Public Member Functions (Specialization for double) ====================
// ======================================================================================
#if 0

// ### This AVX code is actually slower than the GCC's auto-vectorization code  ###

#if defined(PT_GFX_USE_AVX1)

template <>
void BasicAffineMatrix2D<double>::transformPoints(double* dxy, const double* sxy, size_t pointCount) const
{
    pointCount *= 2;

    if(_isIdentity) {
        memcpy(dxy, sxy, pointCount * sizeof(double));
        return;
    }

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    // Loop through 4 doubles at a time
    const size_t pointCount4 = pointCount / 4;

    for(size_t i = 0; i < pointCount4; ++i) {
        // Load 4 doubles from the source vector                            // [ --1-- --0-- ]
        const __m256d s10 = _mm256_loadu_pd       (sxy                   ); // [ Y1 X1 Y0 X0 ]
        const __m256d s1  = _mm256_permute2f128_pd(s10, avxOneZeroD, 0x31); // [ 0  1  Y1 X1 ]
        const __m256d s0  = _mm256_permute2f128_pd(s10, avxOneZeroD, 0x20); // [ 0  1  Y0 X0 ]
        // Multiply them to the matrix's rows                // [ RC RC RC RC ]
        const __m256d r1_0 = _mm256_mul_pd(_mdata.r[0], s1); // [ 03 02 01 00 ]
        const __m256d r1_1 = _mm256_mul_pd(_mdata.r[1], s1); // [ 13 12 11 10 ]
        const __m256d r0_0 = _mm256_mul_pd(_mdata.r[0], s0); // [ 03 02 01 00 ]
        const __m256d r0_1 = _mm256_mul_pd(_mdata.r[1], s0); // [ 13 12 11 10 ]
        // Horizontal add the multiplication results
        const __m256d r1x = _mm256_hadd_pd(r1_0, r1_1);
        const __m256d r0x = _mm256_hadd_pd(r0_0, r0_1);
        // Permute and further add the multiplication results
        const __m256d r1  = _mm256_permute2f128_pd(r1x, r0x, 0x31);
        const __m256d r0  = _mm256_permute2f128_pd(r1x, r0x, 0x02);
        const __m256d r10 = _mm256_add_pd(r1, r0);
        // Store 4 doubles to the destination vector
        _mm256_storeu_pd(
            dxy,
            _mm256_or_pd(
                _mm256_and_pd(s10, _mm256_cmp_pd(s10, avxMaxCordD, _CMP_GT_OQ)), // Retain source values >  maximum coordinate
                _mm256_and_pd(r10, _mm256_cmp_pd(s10, avxMaxCordD, _CMP_LE_OQ))  // Retain result values <= maximum coordinate
            )
        );
        // Increment the pointers
        sxy += 4;
        dxy += 4;
    }

    _mm256_zeroupper(); // Prevent transition penalty from AVX <-> SSE because SSE might be used in other part of the code

    // Process the remaining doubles using normal code
    pointCount %= 4;

    for(size_t i = 0; i < pointCount; i += 2) transformPoint(dxy[i], dxy[i + 1], sxy[i], sxy[i + 1]);
}

template <>
void BasicAffineMatrix2D<double>::transformPoints(double* xy, size_t pointCount) const
{ if(!_isIdentity) transformPoints(xy, xy, pointCount); }

template <>
void BasicAffineMatrix2D<double>::transformPoints(PointF* dxy, const PointF* sxy, size_t pointCount) const
{ transformPoints(reinterpret_cast<double*>(dxy), reinterpret_cast<const double*>(sxy), pointCount); }

template <>
void BasicAffineMatrix2D<double>::transformPoints(PointF* xy, size_t pointCount) const
{ transformPoints(reinterpret_cast<double*>(xy), reinterpret_cast<const double*>(xy), pointCount); }

#endif

#endif


//
// For convenience
//
typedef BasicAffineMatrix2D<float> AffineMatrix2D;


} // namespace
} // namespace

#endif
