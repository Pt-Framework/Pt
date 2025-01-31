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

#ifndef PT_GFX_BASIC_IMAGE_H
#define PT_GFX_BASIC_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>
#include <vector>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicImage
{
    public:
        typedef FormatT Format;

        typedef BasicView<FormatT>                 View;
        typedef typename View::Pixel               Pixel;
        typedef typename View::ConstPixel          ConstPixel;
        typedef typename View::PixelIterator       PixelIterator;
        typedef typename View::ConstPixelIterator  ConstPixelIterator;

        typedef typename View::pos_t  pos_t;
        typedef typename View::Point  Point;
        typedef typename View::Size   Size;
        typedef typename View::Rect   Rect;

    public:

        //
        // TODO: Constructors: copy Model into View and View into Image
        //
        
        /*
          BasicImage(const View& view)
          : _view(view)
          { }

          ArgbImage()
          : BasicImage( BasicView<ArgbModel>() )
          { }

          Image(const ImageFormat& format)
          : BasicImage( BasicView<ImageModel>( ImageModel(format) ) )
          { }
        */

        BasicImage()
        : _view()
        { }

        virtual ~BasicImage()
        {}

        void reset(const Format& format, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding = 0)
        { 
            _buffer.resize( format.imageSize(width, height, padding) );
            
            _view.reset( format, _buffer.empty() ? 0 : &_buffer[0], 
                         width, height, padding );
        }

        void reset(const Format& format, Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        {
           _view.reset(format, data, width, height, padding);
        }

        Pt::ssize_t width() const
        { return _view.width(); }

        Pt::ssize_t height() const
        { return _view.height(); }

        Pt::ssize_t padding() const
        { return _view.padding(); }

        Pt::uint8_t* data()
        { return _view.data(); }

        const Pt::uint8_t* data() const
        { return _view.data(); }

        bool empty() const
        { return _view.empty(); }

        void clear()
        {
            _buffer.clear();
            _view.clear();
        }

        View& view()
        { return _view; }

        const View& view() const
        { return _view; }

        /** @brief Returns an iterator to the pixel at the given position.
        */
        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return _view.pixel(x, y); }

        /** @brief Returns an iterator to the first pixel.
        */
        PixelIterator begin()
        { return _view.begin(); }

        /** @brief Returns an iterator to the end of the pixels.
        */
        PixelIterator end()
        { return _view.end(); }

        /** @brief Returns a const iterator to the pixel at the given position.
        */
        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return _view.pixel(x, y); }

        /** @brief Returns a const iterator to the first pixel.
        */
        ConstPixelIterator begin() const
        { return _view.begin(); }

        /** @brief Returns a const iterator to the end of the pixels.
        */
        ConstPixelIterator end() const
        { return _view.end(); }

    private:
        std::vector<Pt::uint8_t> _buffer;
        View                     _view;
};

} // namespace

} // namespace

#endif
