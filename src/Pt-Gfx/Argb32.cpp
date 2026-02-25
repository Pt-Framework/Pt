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
        explicit Argb32PixelBase(Pt::uint8_t* data, const ViewBase& view, 
                                 Pt::ssize_t x, Pt::ssize_t y)
        : PixelBase(view, Argb32::getPixel(view, data, x, y), x, y )
        { }

    protected:
        virtual PixelBase* onClone(PixelStorage& store) const
        {
            return store.create<Argb32PixelBase>(*this);
        }

        virtual Pt::uint8_t* onAdvance() override
        {
            return Argb32::advance(view(), base());
        }

        virtual Pt::uint8_t* onAdvanceLine() override
        {
            return Argb32::advanceLine(view(), base());
        }

        virtual Pt::uint8_t* onAdvance(Pt::ssize_t n) override
        {
            return Argb32::advance(view(), base(), n);
        }

        virtual Pt::uint8_t* onAdvanceLines(Pt::ssize_t n) override
        {
            return Argb32::advanceLines(view(), base(), n);
        }

        virtual Color onGetColor() const override
        {
            return Argb32::getColor( base() );
        }

        virtual Argb32Color onGetArgb32Color() const override
        {
            return Argb32Color( base() );
        }

        virtual void onSetColor(const Color& color) override
        {
            Argb32::sourceCopy( base(), color );
        }

        virtual void onSetColor(const Argb32Color& color) override
        {
            Argb32::sourceCopy(base(), color);
        }

        virtual void onGetColors(Color* colors, std::size_t length) const override
        {
            Argb32::getColors(base(), colors, length);
        }

        virtual void onGetColors(Argb32Color* colors, std::size_t length) const override
        {
            Argb32::getColors(base(), colors, length);
        }

        virtual void onAssign(const Argb32Color* colors, std::size_t length) override
        {
            const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>(colors);
            Argb32::sourceCopy(base(), p, length);
        }

        virtual void onAssign(const Color* colors, std::size_t length) override
        {
            Argb32::sourceCopy(base(), colors, length);
        }

        virtual void onFillColor(std::size_t n, const Color& color) override
        {
            Argb32::sourceCopy(base(), n, color);
        }

        virtual bool onAssignPixels(const PixelBase& p, std::size_t length) override;

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const override;
};


///////////////////////////////////////////////////////////////////////
// Argb32PixelBase
///////////////////////////////////////////////////////////////////////

inline bool Argb32PixelBase::onAssignPixels(const PixelBase& p, std::size_t length)
{
    if( typeid(p) == typeid(Argb32PixelBase) )
    {
        const Argb32PixelBase* argb32 = static_cast<const Argb32PixelBase*>(&p);

        Pt::uint8_t* to = base();
        const Pt::uint8_t* from = argb32->base();

        Argb32::sourceCopy(to, from, length);
        return true;
    }

    return false;
}


inline bool Argb32PixelBase::onCopyPixels(PixelBase& p, std::size_t length) const
{
    if( typeid(p) == typeid(Argb32PixelBase) )
    {
        Argb32PixelBase* argb32 = static_cast<Argb32PixelBase*>(&p);

        Pt::uint8_t* to = argb32->base();
        const Pt::uint8_t* from = base();

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


PixelBase* Argb32::onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                 Pt::ssize_t x, Pt::ssize_t y, 
                                 PixelStorage& store) const
{
    return store.create<Argb32PixelBase>(data, view, x, y);
}

} // namespace

} // namespace
