/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/Yuv12.h>

namespace Pt {

namespace Gfx {

class Yuv12PixelBase : public PixelBase
{
    public:
        explicit Yuv12PixelBase(Yuv12Pixel& p)
        : PixelBase( p.ybase(), p.xpos(), p.ypos() )
        , _p(p)
        { }

        virtual Location& onAdvance() override
        {
            _p.advance();
            _loc = Location(_p.ybase(), _p.xpos(), _p.ypos());
            return _loc;
        }

         virtual Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                                        Pt::ssize_t n) override
        {
            _p.advance(n);
            xpos = _p.xpos();
            ypos = _p.ypos();

            return _p.ybase();
        }

        virtual Color onGetColor() const override
        {
            return Yuv12::getColor( _p.y(), _p.u(), _p.v() );
        }

        virtual void onSetColor(const Color& color) override
        {
            _p = color;
        }

        virtual void onFillColor(std::size_t n, const Color& color) override
        {
            //Yuv12::sourceCopy(_p.ybase(), n, color);        
        }

    private:
        Yuv12Pixel _p;
        Location  _loc;
};


class Yuv12ConstPixelBase : public ConstPixelBase
{
    public:
        explicit Yuv12ConstPixelBase(const Yuv12ConstPixel& p)
        : ConstPixelBase( p.ybase(), p.xpos(), p.ypos() )
        , _p(p)
        { }

        virtual const Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos) override
        {
            _p.advance();
            xpos = _p.xpos();
            ypos = _p.ypos();

            return _p.ybase();
        }

        virtual const Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                                             Pt::ssize_t n) override
        {
            _p.advance(n);
            xpos = _p.xpos();
            ypos = _p.ypos();

            return _p.ybase();
        }

        virtual Color onGetColor() const override
        {
            return Yuv12::getColor( _p.y(), _p.u(), _p.v() );
        }

    private:      
        Yuv12ConstPixel _p;
};

///////////////////////////////////////////////////////////////////////
// Yuv12
///////////////////////////////////////////////////////////////////////

Yuv12::Yuv12()
: ImageFormat(1)
{
}


std::size_t Yuv12::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                     std::size_t padding) const
{
    return imageSize(width, height, padding);
}


PixelBase* Yuv12::onCreatePixel(Pt::uint8_t* data, ViewBase& view, 
                                Pt::ssize_t x, Pt::ssize_t y, 
                                PixelStorage& store) const
{
    Yuv12Pixel p(data, view, x, y);;
    return store.create<Yuv12PixelBase>(p);
}


ConstPixelBase* Yuv12::onCreateConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                          Pt::ssize_t x, Pt::ssize_t y, 
                                          PixelStorage& store) const
{
    Yuv12ConstPixel p(data, view, x, y);
    return store.create<Yuv12ConstPixelBase>(p);
}


} // namespace

} // namespace
