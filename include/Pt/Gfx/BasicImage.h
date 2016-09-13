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

#ifndef PT_GFX_BASICIMAGE_H
#define PT_GFX_BASICIMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageView.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Types.h>
#include <vector>

namespace Pt {

namespace Gfx {

template <typename PixelT, typename FormatT>
class BasicImage
{
    public:
        class Iterator
        {
            public:
                Iterator(const ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
                : _pixel(view, xpos, ypos)
                { }

                Iterator& operator=(const Iterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                PixelT operator*()
                { return _pixel; }

                Iterator& operator++()
                {
                    _pixel.advance();
                    return *this; 
                }

                bool operator!=(const Iterator& it) const
                { return _pixel != it._pixel; }
        
                bool operator==(const Iterator& it) const
                { return _pixel == it._pixel; }

            private:
                PixelT _pixel;
        };

    public:
        BasicImage(const Size& size, size_t padding = 0)
        : _buffer( _format.imageSize(size, padding) )
        , _view( _format, 
                 _buffer.empty() ? 0 : &_buffer[0], 
                 size, 
                 padding )
        { }

        BasicImage(Pt::uint8_t* data, const Size& size, Pt::ssize_t padding = 0)
        : _view(_format, data, size, padding)
        { }

        virtual ~BasicImage()
        {}

        const ImageView& view() const
        {
            return _view;
        }

        const FormatT& format() const
        {
            return _format;
        }

        Pt::ssize_t width() const
        {
          return _view.width();
        }

        Pt::ssize_t height() const
        {
          return _view.height();
        }
    
        const Size& size() const
        {
            return _view.size();
        }

        Pt::uint8_t* data()
        { 
            return _view.data(); 
        }

        const Pt::uint8_t* data() const
        { 
            return _view.data(); 
        }

        bool empty() const
        {
            return _view.empty();
        }

        Iterator begin()
        { return Iterator(_view, 0, 0); }

        Iterator end()
        { return Iterator(_view, 0, height()); }

    private:
        FormatT                  _format;
        std::vector<Pt::uint8_t> _buffer;
        ImageView                _view;
};

} // namespace

} // namespace

#endif
