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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_GFX_IMAGEFORMAT_H
#define PT_GFX_IMAGEFORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Gfx {

class CompositionMode
{
    public:
        enum Mode
        {
            SourceCopy = 0, // use source pixel
            SourceOver = 1, // use alpha of source pixel
            AlphaMask = 200 // TODO: image painter specific or obsolete
        };

        CompositionMode(Mode m = SourceCopy)
        : _mode(m)
        {}

        CompositionMode& operator =(Mode m)
        {
            _mode = m;
            return *this;
        }

        operator Pt::uint32_t() const
        { 
            return _mode; 
        }

    private:
        Pt::uint32_t _mode;
};

class ImageInfo;

class PT_GFX_API ImageFormat
{
    public:
        ImageFormat(size_t pixelSize, size_t channels);

        virtual ~ImageFormat();
    
        size_t pixelSize() const
        {
          return _pixelSize;
        }

        size_t channels() const
        {
          return _channels;
        }
        
        virtual void setColor(Pt::uint8_t* pixel, const Color& c,
                              CompositionMode mode) const = 0;

        virtual Color color(const Pt::uint8_t* pixel) const = 0;

        virtual void setPixel(Pt::uint8_t* dst, const Pt::uint8_t* src,
                              CompositionMode mode) const = 0;

        virtual void setSpan(Pt::uint8_t* dst, const Pt::uint8_t* src, 
                             size_t length, CompositionMode mode) const = 0;

        void copy(const ImageInfo& to, const Point& toPoint,
                  const ImageInfo& from, const Rect& fromRect,
                  CompositionMode mode) const;

        bool operator==(const ImageFormat& a) const
        {
          return ( _pixelSize == a._pixelSize && _channels == a._channels );
        }

        bool operator!=(const ImageFormat& a) const
        {
          return ( _pixelSize != a._pixelSize || _channels != a._channels );
        }

        static const ImageFormat& rgb565();
    
        static const ImageFormat& rgb888();
    
        static const ImageFormat& argb8888();

    protected:
        virtual void onCopy(const ImageInfo& to, const Point& toPoint,
                            const ImageInfo& from, const Rect& fromRect,
                            CompositionMode mode) const = 0;

    private:
        size_t _pixelSize;
        size_t _channels;
};

} // namespace

} // namespace

#endif
