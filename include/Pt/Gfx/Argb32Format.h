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

#ifndef PT_GFX_ARGB32FORMAT_H
#define PT_GFX_ARGB32FORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Color.h>

namespace Pt {

namespace Gfx {

class PT_GFX_API Argb32Format : public ImageFormat
{
    public:
        /*constexpr*/ Argb32Format()
        : ImageFormat(4)
        {
        }
    
    protected:
        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const;

    protected:
        virtual Color onGetColor(const View& view, const Pt::uint8_t* base, 
                                 Pt::ssize_t x, Pt::ssize_t y) const;

    protected:
        virtual void onSourceCopy(View& view, PixelBase& to, const Color& c) const;
        
        virtual void onSourceOver(View& view, PixelBase& to, const Color& c) const;


        virtual void onSourceCopy(View& to, PixelBase& pos,
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const;

        virtual void onSourceOver(View& to, PixelBase& pos,
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const;

    protected:
        virtual void onSourceCopy(View& view, PixelBase& to, 
                                  std::size_t n, const Color& c) const;

        virtual void onSourceOver(View& view, PixelBase& to, 
                                  std::size_t n, const Color& c) const;


        virtual void onSourceCopy(View& view, PixelBase& to, std::size_t n, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const;

        virtual void onSourceOver(View& view, PixelBase& to, std::size_t n, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const;

    protected:
        virtual void onSourceCopy(View& view, PixelBase& to, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const;

        virtual void onSourceOver(View& view, PixelBase& to, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const;

        virtual void onSourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                  const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                  Pt::ssize_t width, Pt::ssize_t height) const;

        virtual void onSourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                  const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                  Pt::ssize_t width, Pt::ssize_t height) const;
};

} // namespace

} // namespace

#endif
