/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_SIZE_H
#define PT_GFX_SIZE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <Pt/Math.h>

namespace Pt {

namespace Gfx {

class SizeI;

/** @brief %Size with floating-point width and height.
*/
class Size
{
    public:
        Size()
        : _w(0), _h(0)
        {}

        Size(Float w, Float h)
        : _w(w), _h(h)
        {}

        Size(const Size& other)
        : _w(other._w), _h(other._h)
        {}

        void clear()
        {
            _w = 0;
            _h = 0;
        }

        void set(Float w, Float h)
        {
            _w = w;
            _h = h;
        }

        bool isEmpty() const
        {
            return _w == 0 || _h == 0;
        }

        /** @brief Returns the area (width * height).
        */
        Float area() const
        { return _w * _h; }

        /** @brief Swaps width and height in place.
        */
        void transpose()
        {
            Float tmp = _w;
            _w = _h;
            _h = tmp;
        }

        /** @brief Returns a copy with width and height swapped.
        */
        Size toTransposed() const
        { return Size(_h, _w); }

        /** @brief Constructs from a %SizeI by widening the dimensions.
        */
        explicit Size(const SizeI& s);

        /** @brief Assigns from a %SizeI by widening the dimensions.
        */
        Size& operator=(const SizeI& s);

        /** @brief Rounds each dimension to the nearest integer and returns a %SizeI.
        */
        SizeI round() const;

        /** @brief Floors each dimension and returns a %SizeI.
        */
        SizeI floor() const;

        /** @brief Ceils each dimension and returns a %SizeI.
        */
        SizeI ceil() const;

        Float width() const
        { return _w; }

        Float height() const
        { return _h; }

        void setWidth(Float w)
        { _w = w; }

        void setHeight(Float h)
        { _h = h; }

        void setWidthHeight(Float w, Float h)
        {
            _w = w;
            _h = h;
        }

        const Size& addWidth(Float w)
        {
            _w += w;
            return *this;
        }

        const Size& subWidth(Float w)
        {
            _w -= w;
            return *this;
        }

        const Size& addHeight(Float h)
        {
            _h += h;
            return *this;
        }

        const Size& subHeight(Float h)
        {
            _h -= h;
            return *this;
        }

        void add(const Size& s)
        {
            _w += s._w;
            _h += s._h;
        }

        Size& operator=(const Size& other)
        {
            _w = other._w;
            _h = other._h;
            return *this;
        }

        Size& operator+=(const Size& s)
        {
            _w += s._w;
            _h += s._h;
            return *this;
        }

        bool isEqual(const Size& other, Float eps = FloatNearlyZero) const
        {
            return std::abs(_w - other._w) <= eps &&
                   std::abs(_h - other._h) <= eps;
        }

        bool operator<(const Size& other) const
        {
            return _h < other._h || (_h == other._h && _w < other._w);
        }

        Size operator*(Float v) const
        {
            return Size(_w * v, _h * v);
        }

        Size operator/(Float v) const
        {
            return Size(_w / v, _h / v);
        }

        Size operator+(Float v) const
        {
            return Size(_w + v, _h + v);
        }

        Size operator-(Float v) const
        {
            return Size(_w - v, _h - v);
        }

        Size& operator*=(Float v)
        {
            _w *= v;
            _h *= v;
            return *this;
        }

        Size& operator/=(Float v)
        {
            _w /= v;
            _h /= v;
            return *this;
        }

        Size& operator+=(Float v)
        {
            _w += v;
            _h += v;
            return *this;
        }

        Size& operator-=(Float v)
        {
            _w -= v;
            _h -= v;
            return *this;
        }

    private:
        Float _w;
        Float _h;
};

typedef Size SizeF;

/** @brief %Size with integer width and height.
*/
class SizeI
{
    public:
        SizeI()
        : _w(0), _h(0)
        {}

        SizeI(Int w, Int h)
        : _w(w), _h(h)
        {}

        SizeI(const SizeI& other)
        : _w(other._w), _h(other._h)
        {}

        void clear()
        {
            _w = 0;
            _h = 0;
        }

        void set(Int w, Int h)
        {
            _w = w;
            _h = h;
        }

        bool isEmpty() const
        {
            return _w == 0 || _h == 0;
        }

        /** @brief Returns the area (width * height).
        */
        Int area() const
        { return _w * _h; }

        /** @brief Swaps width and height in place.
        */
        void transpose()
        {
            Int tmp = _w;
            _w = _h;
            _h = tmp;
        }

        /** @brief Returns a copy with width and height swapped.
        */
        SizeI toTransposed() const
        { return SizeI(_h, _w); }

        Int width() const
        { return _w; }

        Int height() const
        { return _h; }

        void setWidth(Int w)
        { _w = w; }

        void setHeight(Int h)
        { _h = h; }

        void setWidthHeight(Int w, Int h)
        {
            _w = w;
            _h = h;
        }

        const SizeI& addWidth(Int w)
        {
            _w += w;
            return *this;
        }

        const SizeI& subWidth(Int w)
        {
            _w -= w;
            return *this;
        }

        const SizeI& addHeight(Int h)
        {
            _h += h;
            return *this;
        }

        const SizeI& subHeight(Int h)
        {
            _h -= h;
            return *this;
        }

        void add(const SizeI& s)
        {
            _w += s._w;
            _h += s._h;
        }

        SizeI& operator=(const SizeI& other)
        {
            _w = other._w;
            _h = other._h;
            return *this;
        }

        SizeI& operator+=(const SizeI& s)
        {
            _w += s._w;
            _h += s._h;
            return *this;
        }

        bool operator==(const SizeI& other) const
        {
            return (_w == other._w && _h == other._h);
        }

        bool operator!=(const SizeI& other) const
        {
            return (_w != other._w || _h != other._h);
        }

        SizeI operator*(Int v) const
        {
            return SizeI(_w * v, _h * v);
        }

        SizeI operator+(Int v) const
        {
            return SizeI(_w + v, _h + v);
        }

        SizeI operator-(Int v) const
        {
            return SizeI(_w - v, _h - v);
        }

        SizeI& operator*=(Int v)
        {
            _w *= v;
            _h *= v;
            return *this;
        }

        SizeI& operator+=(Int v)
        {
            _w += v;
            _h += v;
            return *this;
        }

        SizeI& operator-=(Int v)
        {
            _w -= v;
            _h -= v;
            return *this;
        }

    private:
        Int _w;
        Int _h;
};

inline Size::Size(const SizeI& s)
: _w(static_cast<Float>(s.width())), _h(static_cast<Float>(s.height()))
{}

inline Size& Size::operator=(const SizeI& s)
{
    _w = static_cast<Float>(s.width());
    _h = static_cast<Float>(s.height());
    return *this;
}

inline SizeI Size::round() const
{
    return SizeI(static_cast<Int>(std::lround(_w)),
                 static_cast<Int>(std::lround(_h)));
}

inline SizeI Size::floor() const
{
    return SizeI(static_cast<Int>(std::floor(_w)),
                 static_cast<Int>(std::floor(_h)));
}

inline SizeI Size::ceil() const
{
    return SizeI(static_cast<Int>(std::ceil(_w)),
                 static_cast<Int>(std::ceil(_h)));
}

} //namespace

} //namespace

#endif
