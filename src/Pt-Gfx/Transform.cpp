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

#include <Pt/Gfx/Transform.h>

#include <limits>
#include <cmath>
#include <cstring>

namespace Pt {

namespace Gfx {

Transform::Transform()
: _isIdentity(true)
{
    reset();
}


Transform::Transform(double m11, double m12,
                     double m21, double m22,
                     double dx,  double dy)
{
    set(m11, m12, m21, m22, dx, dy);
}

Transform::~Transform()
{
}


bool Transform::isIdentity() const
{
    return _isIdentity;
}


bool Transform::isAffine() const
{
    return true;
}

bool Transform::isSimple() const
{
    double a = std::abs( m12() );
    double b = std::abs( m21() );

    const double eps = 1e-6;

    return a < eps && b < eps;
}


double Transform::m11() const
{
    return _mdata[0][0];
}


double Transform::m12() const
{
    return _mdata[0][1];
}


double Transform::m21() const
{
    return _mdata[1][0];
}


double Transform::m22() const
{
    return _mdata[1][1];
}


double Transform::dx() const
{
    return _mdata[0][2];
}


double Transform::dy() const
{
    return _mdata[1][2];
}


void Transform::reset()
{
    _mdata[0][0] = 1;
    _mdata[0][1] = 0;
    _mdata[0][2] = 0;
    _mdata[1][0] = 0;
    _mdata[1][1] = 1;
    _mdata[1][2] = 0;
    _isIdentity = true;
}


void Transform::set(double m11, double m12,
                    double m21, double m22,
                    double dx, double dy)
{
    _mdata[0][0] = m11;
    _mdata[0][1] = m12;
    _mdata[0][2] = dx;
    _mdata[1][0] = m21;
    _mdata[1][1] = m22;
    _mdata[1][2] = dy;

    updateIdentity();
}


void Transform::translate(double x, double y)
{
    MatrixData n;

    n[0][0] = 1; n[0][1] = 0; n[0][2] = x;
    n[1][0] = 0; n[1][1] = 1; n[1][2] = y;

    concat(n);
}


void Transform::scale(double x, double y)
{
    if(x == 1 && y == 1)
      return;

    MatrixData n;

    n[0][0] = x; n[0][1] = 0; n[0][2] = 0;
    n[1][0] = 0; n[1][1] = y; n[1][2] = 0;

    concat(n);
}


void Transform::rotateDeg(double angle)
{
    const double r = angle * (pi<double>() / 180);
    rotateRad(r);
}


void Transform::rotateRad(double r)
{
    MatrixData n;

    const double s = ::sin(r);
    const double c = ::cos(r);

    n[0][0] = c;  //m11
    n[0][1] = -s; //m12
    n[0][2] = 0;  //
    n[1][0] = s;  //m21
    n[1][1] = c;  //m22
    n[1][2] = 0;

    concat(n);
}


void Transform::shear(double sh, double sv)
{
    //TODO: optimize this
    shearX(sh);
    shearY(sv);
}


void Transform::shearX(double deg)
{
    MatrixData n;

    const double r = degToRad(deg);
    const double t = ::sin(r) / ::cos(r);

    n[0][0] = 1; n[0][1] = t; n[0][2] = 0;
    n[1][0] = 0; n[1][1] = 1; n[1][2] = 0;

    concat(n);
}


void Transform::shearY(double deg)
{
    MatrixData n;

    const double r = degToRad(deg);
    const double t = ::sin(r) / ::cos(r);

    n[0][0] = 1; n[0][1] = 0; n[0][2] = 0;
    n[1][0] = t; n[1][1] = 1; n[1][2] = 0;

    concat(n);
}


bool Transform::operator==(const Transform& m) const
{
    return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) == 0;
}


bool Transform::operator!=(const Transform& m) const
{
   return memcmp(&_mdata, &m._mdata, sizeof(_mdata)) != 0;
}


Transform& Transform::operator*=(const Transform& rhs)
{
    concat(rhs._mdata);
    return *this;
}


Transform Transform::operator*(const Transform& rhs) const
{
    Transform result = *this;
    result.concat(rhs._mdata);
    return result;
}


PointF Transform::operator*(const PointF& p) const
{
    PointF result;

    result.setX( _mdata[0][0] * p.x() + _mdata[0][1] * p.y() + _mdata[0][2] );
    result.setY( _mdata[1][0] * p.x() + _mdata[1][1] * p.y() + _mdata[1][2] );

    return result;
}


SizeF Transform::operator*(const SizeF& sz) const
{
    SizeF result;
    PointF za(sz.width(), 0);
    PointF zb(0, sz.height());
    PointF r(0, 0);

    za = *this * za;
    zb = *this * zb;
    r  = *this * r;

    const double dxa = za.x() - r.x();
    const double dya = za.y() - r.y();
    const double dxb = zb.x() - r.x();
    const double dyb = zb.y() - r.y();

    result.setWidth( ::sqrt(dxa * dxa + dya * dya) );
    result.setHeight( ::sqrt(dxb * dxb + dyb * dyb) );
    return result;
}


double Transform::determinant() const
{
    return m11() * m22() - m12() * m21();
}


bool Transform::isInvertible() const
{
    return std::abs(determinant()) >= std::numeric_limits<double>::epsilon();
}


Transform Transform::inverted() const
{
    const double det = determinant();

    if ( std::abs(det) < std::numeric_limits<double>::epsilon() )
        return Transform();

    const double invDet = 1.0 / det;

    return Transform(m22() * invDet,
        -m12() * invDet,
        -m21() * invDet,
        m11() * invDet,
        (m12() * dy() - m22() * dx()) * invDet,
        (m21() * dx() - m11() * dy()) * invDet);

}


void Transform::concat(const MatrixData& m)
{
    MatrixData result;

    result[0][0] = _mdata[0][0] * m[0][0] + _mdata[0][1] * m[1][0];
    result[0][1] = _mdata[0][0] * m[0][1] + _mdata[0][1] * m[1][1];
    result[0][2] = _mdata[0][0] * m[0][2] + _mdata[0][1] * m[1][2] + _mdata[0][2];

    result[1][0] = _mdata[1][0] * m[0][0] + _mdata[1][1] * m[1][0];
    result[1][1] = _mdata[1][0] * m[0][1] + _mdata[1][1] * m[1][1];
    result[1][2] = _mdata[1][0] * m[0][2] + _mdata[1][1] * m[1][2] + _mdata[1][2];

    memcpy(_mdata, result, sizeof(MatrixData));
    updateIdentity();
}


void Transform::updateIdentity()
{
    const double eps = 1e-6;
    _isIdentity = std::abs(_mdata[0][0] - 1.0) < eps && 
                  std::abs(_mdata[0][1]) < eps       && 
                  std::abs(_mdata[0][2]) < eps       &&
                  std::abs(_mdata[1][0]) < eps       && 
                  std::abs(_mdata[1][1] - 1.0) < eps && 
                  std::abs(_mdata[1][2]) < eps;
}

} // namespace

} // namespace
