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

#include <Pt/Gfx/Argb32.h>

/*
<Format concept>

class ViewBase;

class BasicView : ViewBase
class BasicPixelIterator

class BasicConstView : ViewBase
class BasicConstPixelIterator


class ImageFormat <Format concept>
class Argb32Format : ImageFormat
class Rgb565Format : ImageFormat
class ImageView : BasicView
class ImageConstView : BasicView


Argb32 <Format concept>
class Argb32View : BasicView
class Argb32ConstView : BasicView

Yuv12 <Format concept>
class Yuv12View : BasicView
class Yuv12ConstView : BasicView


class BasicImage
class Image : BasicImage
class Argb32Image : BasicImage
class Yuv12Image : BasicImage
*/

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Argb32PixelBase
///////////////////////////////////////////////////////////////////////

class Argb32PixelBase final : public PixelBase
{
    public:
        explicit Argb32PixelBase(Argb32Pixel& p)
        : PixelBase( p.base(), p.xpos(), p.ypos() )
        , _p(p)
        { }

    protected:
        virtual Location& onAdvance() override
        {
            _p.advance();
            return _p.location();
        }

         virtual Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                                        Pt::ssize_t n) override
        {
            _p.advance(n);
            xpos = _p.xpos();
            ypos = _p.ypos();

            return _p.base();
        }

        virtual Color onGetColor() const override
        {
            return Argb32::getColor( _p.base() );
        }

        virtual Argb32Color onGetArgb32Color() const override
        {
            return Argb32Color( _p.base() );
        }

        virtual void onSetColor(const Color& color) override
        {
            _p = color;
        }

        virtual void onSetColor(const Argb32Color& color) override
        {
            _p = *color.value();
        }

        virtual void onFillColor(std::size_t n, const Color& color) override
        {
            Argb32::sourceCopy(_p.base(), n, color);
        }

        virtual bool onAssignPixels(const ConstPixelBase& p, std::size_t length) override;

    private:      
        Argb32Pixel _p;
};

///////////////////////////////////////////////////////////////////////
// Argb32ConstPixelBase
///////////////////////////////////////////////////////////////////////

class Argb32ConstPixelBase final : public ConstPixelBase
{
    public:
        explicit Argb32ConstPixelBase(const Argb32ConstPixel& p)
        : ConstPixelBase( p.base(), p.xpos(), p.ypos() )
        , _p(p)
        { }

    protected:
        virtual const Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos) override
        {
            _p.advance();
            xpos = _p.xpos();
            ypos = _p.ypos();

            return _p.base();
        }

         virtual const Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                                              Pt::ssize_t n) override
        {
            _p.advance(n);
            xpos = _p.xpos();
            ypos = _p.ypos();

            return _p.base();
        }

        virtual Color onGetColor() const override
        {
            return Argb32::getColor( _p.base() );
        }

        virtual Argb32Color onGetArgb32Color() const override
        {
            return Argb32Color( _p.base() );
        }

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const override;

    private:      
        Argb32ConstPixel _p;
};

///////////////////////////////////////////////////////////////////////
// Argb32PixelBase
///////////////////////////////////////////////////////////////////////

inline bool Argb32PixelBase::onAssignPixels(const ConstPixelBase& p, std::size_t length)
{
    if( typeid(p) == typeid(Argb32ConstPixelBase) )
    {
        const Argb32ConstPixelBase* argb32 = static_cast<const Argb32ConstPixelBase*>(&p);

        Pt::uint8_t* to = base();
        const Pt::uint8_t* from = argb32->base();

        Argb32::sourceCopy(to, from, length);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// Argb32ConstPixelBase
///////////////////////////////////////////////////////////////////////

inline bool Argb32ConstPixelBase::onCopyPixels(PixelBase& p, std::size_t length) const
{
    if( typeid(p) == typeid(Argb32PixelBase) )
    {
        Argb32PixelBase* argb32 = static_cast<Argb32PixelBase*>(&p);

        Pt::uint8_t* to = argb32->base();
        const Pt::uint8_t* from = _p.base();

        Argb32::sourceCopy(to, from, length);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// Argb32
///////////////////////////////////////////////////////////////////////

std::size_t Argb32::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                      std::size_t padding) const
{
    return imageSize(width, height, padding);
}


PixelBase* Argb32::onCreatePixel(Pt::uint8_t* data, ViewBase& view, 
                                 Pt::ssize_t x, Pt::ssize_t y, 
                                 PixelStorage& store) const
{
    Argb32Pixel p(data, view, x, y);
    return store.create<Argb32PixelBase>(p);
}


ConstPixelBase* Argb32::onCreateConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                           Pt::ssize_t x, Pt::ssize_t y, 
                                           PixelStorage& store) const
{
    Argb32ConstPixel p(data, view, x, y);
    return store.create<Argb32ConstPixelBase>(p);
}

} // namespace

} // namespace
