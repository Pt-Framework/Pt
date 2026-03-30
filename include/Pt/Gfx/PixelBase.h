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
#include <Pt/Types.h>
#include <utility>

namespace Pt {

namespace Gfx {

class PixelStorage;

///////////////////////////////////////////////////////////////////////
// PixelBase
///////////////////////////////////////////////////////////////////////

/** @brief Pixel base class.
*/
class PixelBase
{
    public:
        PixelBase(const ViewBase& view, Pt::uint8_t* base, Pt::ssize_t x, Pt::ssize_t y)
        : _view(&view)
        , _base(base)
        { }

        virtual ~PixelBase()
        { }

        const ViewBase& view() const
        { return *_view; }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        void advance()
        { 
            _base = onAdvance();
        }

        void advance(Pt::ssize_t n)
        { 
            _base = onAdvance(n);
        }

        void skipPadding()
        {
            _base = onSkipPadding();
        }

        void advanceLines(Pt::ssize_t n)
        {
            _base = onAdvanceLines(n);
        }

        template <typename ColorT>
        ColorT toColor() const;

        void getColors(ColorF* colors, std::size_t length) const
        { 
            onGetColors(colors, length); 
        }

        void getColors(Argb32Color* colors, std::size_t length) const
        { 
            onGetColors(colors, length); 
        }

        void assign(const ColorF& color)
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

        void assign(const ColorF* colors, std::size_t length)
        { 
            onAssign(colors, length); 
        }

        bool assign(const PixelBase& p, std::size_t length)
        {
            return onAssignPixels(p, length);
        }

        void fill(std::size_t n, const ColorF& color)
        {
            onFillColor(n, color);
        }

        void fill(std::size_t n, const Argb32Color& color)
        {
            onFillColor(n, color);
        }

        bool copy(PixelBase& p, std::size_t length) const
        {
            return onCopyPixels(p, length);
        }

        PixelBase* clone(PixelStorage& store) const
        {
            return onClone(store);
        }

    protected:
        virtual PixelBase* onClone(PixelStorage& store) const
        { return 0; }

        virtual Pt::uint8_t* onAdvance() = 0;

        virtual Pt::uint8_t* onSkipPadding()
        { return 0; }

        virtual Pt::uint8_t* onAdvance(Pt::ssize_t n) = 0;

        virtual Pt::uint8_t* onAdvanceLines(Pt::ssize_t n)
        { return 0; }

        virtual ColorF onGetColor() const = 0;

        virtual Argb32Color onGetArgb32Color() const = 0;

        virtual void onGetColors(ColorF* colors, std::size_t length) const = 0;

        virtual void onGetColors(Argb32Color* colors, std::size_t length) const = 0;

        virtual void onSetColor(const ColorF& color)
        {
            onFillColor(1, color);
        }

        virtual void onSetColor(const Argb32Color& color)
        {
            onFillColor(1, color);
        }

        virtual void onAssign(const Argb32Color* colors, std::size_t length) = 0;

        virtual void onAssign(const ColorF* colors, std::size_t length) = 0;

        virtual void onFillColor(std::size_t n, const ColorF& color) = 0;

        virtual void onFillColor(std::size_t n, const Argb32Color& color) = 0;

        virtual bool onAssignPixels(const PixelBase& p, std::size_t length)
        { return false; }

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const
        { return false; }

    private:
        const ViewBase* _view;
        Pt::uint8_t*    _base;
};


template <typename ColorT>
inline ColorT PixelBase::toColor() const
{
    return this->onGetColor();
}


template <>
inline Argb32Color PixelBase::toColor<Argb32Color>() const
{
    return this->onGetArgb32Color();
}

///////////////////////////////////////////////////////////////////////
// PixelStorage
///////////////////////////////////////////////////////////////////////

class PixelStorage
{
    public:
        static const std::size_t MaxSize = sizeof(void*) * 16;
    
    public:
        template <typename T, typename... Args>
        T* create(Args&&... args)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (&_data.mem) T(std::forward<Args>(args)...);
        }

    private:
        union Data
        {
            Data()
            : ptr(0)
            { }

            alignas(PixelBase) char mem[PixelStorage::MaxSize];
            void* ptr;
            void (*_align0)();
            std::size_t _align1;
            long double _align2;
        } _data;
};

} // namespace

} // namespace

#endif
