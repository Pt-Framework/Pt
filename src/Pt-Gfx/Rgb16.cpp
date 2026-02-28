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

#include <Pt/Gfx/Rgb16.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Rgb16PixelBase
///////////////////////////////////////////////////////////////////////

class Rgb16PixelBase final : public PixelBase
{
    public:
        explicit Rgb16PixelBase(Pt::uint8_t* data, const ViewBase& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
        : PixelBase(view, Rgb16::getPixel(view, data, x, y), x, y)
        { }

    protected:
        virtual PixelBase* onClone(PixelStorage& store) const override
        {
            return store.create<Rgb16PixelBase>(*this);
        }

        virtual Pt::uint8_t* onAdvance() override
        {
            return Rgb16::advance(view(), base());
        }

        virtual Pt::uint8_t* onSkipPadding() override
        {
            return Rgb16::skipPadding(view(), base());
        }

        virtual Pt::uint8_t* onAdvance(Pt::ssize_t n) override
        {
            return Rgb16::advance(view(), base(), n);
        }

        virtual Pt::uint8_t* onAdvanceLines(Pt::ssize_t n) override
        {
            return Rgb16::advanceLines(view(), base(), n);
        }

        virtual ColorF onGetColor() const override
        {
            return Rgb16::getColor(base());
        }

        virtual Argb32Color onGetArgb32Color() const override
        {
            return Rgb16::getArgb32Color(base());
        }

        virtual void onSetColor(const ColorF& color) override
        {
            Rgb16::sourceCopy(base(), color);
        }

        virtual void onSetColor(const Argb32Color& color) override
        {
            Rgb16::sourceCopy(base(), color);
        }

        virtual void onGetColors(ColorF* colors, std::size_t length) const override
        {
            Rgb16::getColors(base(), colors, length);
        }

        virtual void onGetColors(Argb32Color* colors, std::size_t length) const override
        {
            Rgb16::getColors(base(), colors, length);
        }

        virtual void onAssign(const Argb32Color* colors, std::size_t length) override
        {
            Pt::uint8_t* p = base();
            
            for(std::size_t i = 0; i < length; ++i)
            {
                Rgb16::sourceCopy(p, colors[i]);
                p += 2;
            }
        }

        virtual void onAssign(const ColorF* colors, std::size_t length) override
        {
            Rgb16::sourceCopy(base(), colors, length);
        }

        virtual void onFillColor(std::size_t n, const ColorF& color) override
        {
            Rgb16::sourceCopy(base(), n, color);
        }

        virtual bool onAssignPixels(const PixelBase& p, std::size_t length) override;

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const override;
};


inline bool Rgb16PixelBase::onAssignPixels(const PixelBase& p, std::size_t length)
{
    if( typeid(p) == typeid(Rgb16PixelBase) )
    {
        const Rgb16PixelBase* rgb16 = static_cast<const Rgb16PixelBase*>(&p);

        Pt::uint8_t* to = base();
        const Pt::uint8_t* from = rgb16->base();

        Rgb16::sourceCopy(to, from, length);
        return true;
    }

    return false;
}


inline bool Rgb16PixelBase::onCopyPixels(PixelBase& p, std::size_t length) const
{
    if( typeid(p) == typeid(Rgb16PixelBase) )
    {
        Rgb16PixelBase* rgb16 = static_cast<Rgb16PixelBase*>(&p);

        Pt::uint8_t* to = rgb16->base();
        const Pt::uint8_t* from = base();

        Rgb16::sourceCopy(to, from, length);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// Rgb16
///////////////////////////////////////////////////////////////////////

std::size_t Rgb16::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                               std::size_t padding) const
{
    return imageSize(width, height, padding);
}


PixelBase* Rgb16::onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                Pt::ssize_t x, Pt::ssize_t y, 
                                PixelStorage& store) const
{
    return store.create<Rgb16PixelBase>(data, view, x, y);
}

} // namespace

} // namespace
