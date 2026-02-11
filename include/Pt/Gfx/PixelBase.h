/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_GFX_PIXELBASE_H
#define PT_GFX_PIXELBASE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Location.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// PixelBase
///////////////////////////////////////////////////////////////////////

class PixelBase;
class ConstPixelBase;

/** @brief Pixel base class.
*/
class PixelBase
{
    public:
        PixelBase(Pt::uint8_t* base, Pt::ssize_t x, Pt::ssize_t y)
        : _base(base)
        { }

        virtual ~PixelBase()
        { }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        Location& advance()
        { 
            Location& loc = onAdvance();
            _base = loc.base();
            return loc;
        }

        Location& advance(Pt::ssize_t n)
        { 
            Location& loc = onAdvance(n); 
            _base = loc.base();
            return loc;
        }

        Color toColor() const
        {
            return this->onGetColor();
        }

        Argb32Color toArgb32Color() const
        {
            return this->onGetArgb32Color();
        }

        void getColors(Color* colors, std::size_t length) const
        { 
            onGetColors(colors, length); 
        }

        void getColors(Argb32Color* colors, std::size_t length) const
        { 
            onGetColors(colors, length); 
        }

        void assign(const Color& color)
        { 
            onSetColor(color);
        }

        void assign(const Argb32Color& color)
        { 
            onSetColor(color);
        }
        
        void assign(const Argb32Color* colors, std::size_t length)
        { 
            onAssign(colors, length); 
        }

        void assign(const Color* colors, std::size_t length)
        { 
            onAssign(colors, length); 
        }

        bool assign(const ConstPixelBase& p, std::size_t length)
        {
            return onAssignPixels(p, length);
        }

        void fill(std::size_t n, const Color& color)
        {
            onFillColor(n, color);
        }

    protected:
        virtual Location& onAdvance() = 0;

        virtual Location& onAdvance(Pt::ssize_t n) = 0;

        virtual Color onGetColor() const = 0;

        virtual Argb32Color onGetArgb32Color() const
        { return Argb32Color(); }

        virtual void onGetColors(Color* colors, std::size_t length) const
        { }

        virtual void onGetColors(Argb32Color* colors, std::size_t length) const
        { }

        virtual void onSetColor(const Color& color) = 0;

        virtual void onSetColor(const Argb32Color& color)
        { }

        virtual void onAssign(const Argb32Color* colors, std::size_t length)
        { }

        virtual void onAssign(const Color* colors, std::size_t length)
        { }

        virtual void onAssign(Argb32Color* colors, std::size_t length)
        { }

        virtual void onFillColor(std::size_t n, const Color& color) = 0;

        virtual bool onAssignPixels(const ConstPixelBase& p, std::size_t length)
        { return false; }

    private:
        Pt::uint8_t* _base;
};

///////////////////////////////////////////////////////////////////////
// ConstPixelBase
///////////////////////////////////////////////////////////////////////

/** @brief Const pixel base class.
*/
class ConstPixelBase
{
    public:
        ConstPixelBase(const Pt::uint8_t* base, Pt::ssize_t x, Pt::ssize_t y)
        : _base(base)
        { }

        virtual ~ConstPixelBase()
        {}

        const Pt::uint8_t* base() const
        { return _base; }

        const ConstLocation& advance()
        { 
            const ConstLocation& loc = onAdvance();
            _base = loc.base();
            return loc;
        }

        const ConstLocation& advance(Pt::ssize_t n)
        { 
            const ConstLocation& loc = onAdvance(n); 
            _base = loc.base();
            return loc;
        }

        Color toColor() const
        {
            return this->onGetColor();
        }

        Argb32Color toArgb32Color() const
        {
            return this->onGetArgb32Color();
        }

        void getColors(Color* colors, std::size_t length) const
        { 
            onGetColors(colors, length); 
        }

        void getColors(Argb32Color* colors, std::size_t length) const
        { 
            onGetColors(colors, length); 
        }

        bool copy(PixelBase& p, std::size_t length) const
        {
            return onCopyPixels(p, length);
        }

    protected:
        virtual const ConstLocation& onAdvance() = 0;

        virtual const ConstLocation& onAdvance(Pt::ssize_t n) = 0;

        virtual Color onGetColor() const = 0;

        virtual Argb32Color onGetArgb32Color() const
        { return Argb32Color(); }

        virtual void onGetColors(Color* colors, std::size_t length) const
        { }

        virtual void onGetColors(Argb32Color* colors, std::size_t length) const
        { }

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const
        { return false; }

    private:
        const Pt::uint8_t*   _base;
};

///////////////////////////////////////////////////////////////////////
// PixelStorage
///////////////////////////////////////////////////////////////////////

class PixelStorage
{
    public:
        static const std::size_t MaxSize = 128;
    
    public:
        template <typename T>
        T* create(Pt::uint8_t* data, ViewBase& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (_data.mem) T(data, view, x, y);
        }

        template <typename T>
        T* create(const Pt::uint8_t* data, const ViewBase& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (_data.mem) T(data, view, x, y);
        }

        template <typename T, typename P>
        T* create(P& p)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (_data.mem) T(p);
        }

    private:
        union Data
        {
            Data()
            : ptr(0)
            { }

            char  mem[PixelStorage::MaxSize];
            void* ptr;
            std::size_t _align1;
            long double _align2;
        } _data;
};

} // namespace

} // namespace

#endif
