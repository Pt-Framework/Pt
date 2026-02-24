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

#include <Pt/Gfx/Rgb16.h>

 namespace Pt {

 namespace Gfx {

Rgb16::Rgb16()
: ImageFormat(2)
{
}


std::size_t Rgb16::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                     std::size_t padding) const
{
    std::size_t l = (width * 2) + padding;
    std::size_t n = l * height;
    return n;
}

PixelBase* Rgb16::onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                  Pt::ssize_t x, Pt::ssize_t y, 
                                  PixelStorage& store) const
{ 
    return 0; 
}

//
// Get pixel color
//
//
//Color Rgb16Format::onGetColor(const View& view, const Pt::uint8_t* base, 
//                              Pt::ssize_t x, Pt::ssize_t y) const
//{
//    const Pt::uint16_t* p = (const Pt::uint16_t*) base;
//
//    const uint16_t tr = (*p & 0xF800) >> 11;
//    const uint16_t tg = (*p & 0x07E0) >> 5;
//    const uint16_t tb = *p & 0x001F;
//
//    uint16_t a = 0xFFFF;
//    uint16_t r = ((tr + !!tr) << 11) - !!tr;
//    uint16_t g = ((tg + !!tg) << 10) - !!tg;
//    uint16_t b = ((tb + !!tb) << 11) - !!tb;
//
//    return Color(a, r, g, b);
//}
//
////
//// Assign pixel
////
//
//void Rgb16Format::onSourceCopy(View& view, PixelBase& to, const Color& c) const
//{
//    Pt::uint32_t val =   uint32_t(c.red() & 0xF800) |
//                       ( uint32_t(c.green() & 0xFC00) >> 5 ) |
//                       ( uint32_t(c.blue () ) >> 11 );
//
//    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>( to.base() );
//    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)val);
//}
//
//
//void Rgb16Format::onSourceOver(View& view, PixelBase& to, const Color& c) const
//{
//    Pt::uint32_t val =   uint32_t(c.red() & 0xF800) |
//                       ( uint32_t(c.green() & 0xFC00) >> 5 ) |
//                       ( uint32_t(c.blue () ) >> 11 );
//
//    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>( to.base() );
//    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)val);
//}
//
//
//void Rgb16Format::onSourceCopy(View& to, PixelBase& pos,
//                                const View& from, const Pt::uint8_t* base,
//                                Pt::ssize_t x, Pt::ssize_t y) const
//{
//    Pt::uint8_t* dst = pos.base();
//    const Pt::uint8_t* src = base;
//
//    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)src);
//}
//
//
//void Rgb16Format::onSourceOver(View& to, PixelBase& pos,
//                                const View& from, const Pt::uint8_t* base,
//                                Pt::ssize_t x, Pt::ssize_t y) const
//{
//    Pt::uint8_t* dst = pos.base();
//    const Pt::uint8_t* src = base;
//
//    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)src);
//}
//
////
//// Fill pixels
////
//
//void Rgb16Format::onSourceCopy(View& view, PixelBase& to, 
//                                std::size_t n, const Color& c) const
//{
//    // ### !!! TODO !!! ###
//}
//
//
//void Rgb16Format::onSourceOver(View& view, PixelBase& to, 
//                                std::size_t n, const Color& c) const
//{
//    // ### !!! TODO !!! ###
//}
//
//
//void Rgb16Format::onSourceCopy(View& view, PixelBase& to, std::size_t n, 
//                                const View& from, const Pt::uint8_t* base,
//                                Pt::ssize_t x, Pt::ssize_t y) const
//{
//    // ### !!! TODO !!! ###
//}
//
//
//void Rgb16Format::onSourceOver(View& view, PixelBase& to, std::size_t n, 
//                                const View& from, const Pt::uint8_t* base,
//                                Pt::ssize_t x, Pt::ssize_t y) const
//{
//    // ### !!! TODO !!! ###
//}
//
////
//// Copy pixels
////
//
//void Rgb16Format::onSourceCopy(View& view, PixelBase& to, 
//                                const View& from, const Pt::uint8_t* base,
//                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
//{
//    std::memcpy(to.base(), base, n * 2);
//}
//
//
//void Rgb16Format::onSourceOver(View& view, PixelBase& to, 
//                                const View& from, const Pt::uint8_t* base,
//                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
//{
//    std::memcpy(to.base(), base, n * 2);
//}
//
//
//void Rgb16Format::onSourceCopy(View& toView, Pt::ssize_t toX, Pt::ssize_t toY,
//                               const View& fromView, Pt::ssize_t fromX, Pt::ssize_t fromY,
//                               Pt::ssize_t width, Pt::ssize_t height) const
//{
//    assert( toX >= 0 && toY >= 0 &&
//            toX + width <= toView.width() &&
//            toY + height<= toView.height() );
//
//    Pt::ssize_t bytesPerPixel = 2;
//    Pt::ssize_t n = width * bytesPerPixel;
//
//    Pt::uint8_t* to = toView.data() + (toY * toView.stride()) 
//                                    + (toX * bytesPerPixel);
//    
//    const Pt::uint8_t* from = fromView.data() + (fromY * fromView.stride()) 
//                                              + (fromX * bytesPerPixel);
//
//    for(Pt::ssize_t y = 0; y < height; ++y)
//    {
//        memcpy(to, from, n);
//
//        to += toView.stride();
//        from += fromView.stride();
//    }
//}
//
//
//void Rgb16Format::onSourceOver(View& toView, Pt::ssize_t toX, Pt::ssize_t toY,
//                               const View& fromView, Pt::ssize_t fromX, Pt::ssize_t fromY,
//                               Pt::ssize_t width, Pt::ssize_t height) const
//{
//    assert( toX >= 0 && toY >= 0 &&
//            toX + width <= toView.width() &&
//            toY + height<= toView.height() );
//
//    Pt::ssize_t bytesPerPixel = 2;
//    Pt::ssize_t n = width * bytesPerPixel;
//
//    Pt::uint8_t* to = toView.data() + (toY * toView.stride()) 
//                                    + (toX * bytesPerPixel);
//    
//    const Pt::uint8_t* from = fromView.data() + (fromY * fromView.stride()) 
//                                              + (fromX * bytesPerPixel);
//
//    for(Pt::ssize_t y = 0; y < height; ++y)
//    {
//        memcpy(to, from, n);
//
//        to += toView.stride();
//        from += fromView.stride();
//    }
//}

} // namespace

} // namespace
