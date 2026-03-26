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
#include <stdexcept>

namespace Pt {

namespace Gfx {

class Yuv12PixelBase final : public PixelBase
{
    public:
        explicit Yuv12PixelBase(const Yuv12Pixel& p)
        : PixelBase(p.view(), p.ybase(), p.xpos(), p.ypos() )
        , _p(p)
        { }

        const Pt::uint8_t* ybase() const
        { return _p.ybase(); }

        const Pt::uint8_t* ubase() const
        { return _p.ubase(); }

        const Pt::uint8_t* vbase() const
        { return _p.vbase(); }

    protected:
        virtual PixelBase* onClone(PixelStorage& store) const
        {
            const Yuv12PixelBase& c = *this;
            return store.create<Yuv12PixelBase>(c);
        }

        virtual Pt::uint8_t* onAdvance() override
        {
            _p.advance();
            return _p.ybase();
        }

        virtual Pt::uint8_t* onSkipPadding() override
        {
            return _p.ybase();
        }

        virtual Pt::uint8_t* onAdvance(Pt::ssize_t n) override
        {
            _p.advance(n);
            return _p.ybase();
        }

        virtual Pt::uint8_t* onAdvanceLines(Pt::ssize_t n) override
        {
            Pt::ssize_t m = n * _p.view().width();
            _p.advance(m);
            return _p.ybase();
        }

        virtual ColorF onGetColor() const override
        {
            return Yuv12::getColor( _p.y(), _p.u(), _p.v() );
        }

        virtual void onSetColor(const ColorF& color) override
        {
            _p = color;
        }

        virtual void onFillColor(std::size_t n, const ColorF& color) override
        {
            //Yuv12::sourceCopy(_p.ybase(), n, color);        
        }

        virtual bool onAssignPixels(const PixelBase& p, std::size_t length) override;

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const override;

    private:
        Yuv12Pixel _p;
};

///////////////////////////////////////////////////////////////////////
// Yuv12PixelBase
///////////////////////////////////////////////////////////////////////

inline bool Yuv12PixelBase::onAssignPixels(const PixelBase& p, std::size_t length)
{
    if( typeid(p) == typeid(Yuv12PixelBase) )
    {
        const Yuv12PixelBase* yuv = static_cast<const Yuv12PixelBase*>(&p);
        return true;
    }

    return false;
}


inline bool Yuv12PixelBase::onCopyPixels(PixelBase& p, std::size_t length) const
{
    if( typeid(p) == typeid(Yuv12PixelBase) )
    {
        //Yuv12PixelBase* yuv12 = static_cast<Yuv12PixelBase*>(&p);
        return true;
    }

    return false;
}

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


PixelBase* Yuv12::onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                Pt::ssize_t x, Pt::ssize_t y, 
                                PixelStorage& store) const
{
    Yuv12Pixel p(data, view, x, y);;
    return store.create<Yuv12PixelBase>(p);
}

} // namespace

} // namespace
