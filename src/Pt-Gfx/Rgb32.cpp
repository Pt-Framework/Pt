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

#include <Pt/Gfx/Rgb32.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Rgb32PixelBase
///////////////////////////////////////////////////////////////////////

class Rgb32PixelBase final : public PixelBase
{
    public:
        explicit Rgb32PixelBase(Pt::uint8_t* data, const ViewBase& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
        : PixelBase(view, Rgb32::getPixel(view, data, x, y), x, y)
        { }

    protected:
        virtual PixelBase* onClone(PixelStorage& store) const override
        {
            return store.create<Rgb32PixelBase>(*this);
        }

        virtual Pt::uint8_t* onAdvance() override
        {
            return Rgb32::advance(view(), base());
        }

        virtual Pt::uint8_t* onSkipPadding() override
        {
            return Rgb32::skipPadding(view(), base());
        }

        virtual Pt::uint8_t* onAdvance(Pt::ssize_t n) override
        {
            return Rgb32::advance(view(), base(), n);
        }

        virtual Pt::uint8_t* onAdvanceLines(Pt::ssize_t n) override
        {
            return Rgb32::advanceLines(view(), base(), n);
        }

        virtual ColorF onGetColor() const override
        {
            return Rgb32::toColor( base() );
        }

        virtual Argb32Color onGetArgb32Color() const override
        {
            return Rgb32::toArgb32Color( base() );
        }

        virtual void onSetColor(const ColorF& color) override
        {
            Rgb32::assign( base(), color );
        }

        virtual void onSetColor(const Argb32Color& color) override
        {
            Rgb32::assign(base(), color);
        }

        virtual void onGetColors(ColorF* colors, std::size_t length) const override
        {
            Rgb32::getColors(base(), colors, length);
        }

        virtual void onGetColors(Argb32Color* colors, std::size_t length) const override
        {
            Rgb32::getColors(base(), colors, length);
        }

        virtual void onAssign(const Argb32Color* colors, std::size_t length) override
        {
            Rgb32::assign(base(), colors, length);
        }

        virtual void onAssign(const ColorF* colors, std::size_t length) override
        {
            Rgb32::assign(base(), colors, length);
        }

        virtual void onFillColor(std::size_t n, const ColorF& color) override
        {
            Rgb32::fill(base(), n, color);
        }

        virtual void onFillColor(std::size_t n, const Argb32Color& color) override
        {
            Rgb32::fill(base(), n, color);
        }

        virtual bool onAssignPixels(const PixelBase& p, std::size_t length) override;

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const override;
};


inline bool Rgb32PixelBase::onAssignPixels(const PixelBase& p, std::size_t length)
{
    if( typeid(p) == typeid(Rgb32PixelBase) )
    {
        const Rgb32PixelBase* rgb32 = static_cast<const Rgb32PixelBase*>(&p);

        Pt::uint8_t* to = base();
        const Pt::uint8_t* from = rgb32->base();

        Rgb32::copy(to, from, length);
        return true;
    }

    return false;
}


inline bool Rgb32PixelBase::onCopyPixels(PixelBase& p, std::size_t length) const
{
    if( typeid(p) == typeid(Rgb32PixelBase) )
    {
        Rgb32PixelBase* rgb32 = static_cast<Rgb32PixelBase*>(&p);

        Pt::uint8_t* to = rgb32->base();
        const Pt::uint8_t* from = base();

        Rgb32::copy(to, from, length);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// Rgb32
///////////////////////////////////////////////////////////////////////

std::size_t Rgb32::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                               std::size_t padding) const
{
    return imageSize(width, height, padding);
}


PixelBase* Rgb32::onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                Pt::ssize_t x, Pt::ssize_t y, 
                                PixelStorage& store) const
{
    return store.create<Rgb32PixelBase>(data, view, x, y);
}

} // namespace

} // namespace
