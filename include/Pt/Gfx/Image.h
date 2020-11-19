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

#ifndef PT_GFX_IMAGE_H
#define PT_GFX_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageView.h>
#include <vector>

namespace Pt {

namespace Gfx {

/** @brief Generic image.
*/
class PT_GFX_API Image
{
    public:
        typedef ImageView::Pixel Pixel;
        typedef ImageView::ConstPixel ConstPixel;
        typedef ImageView::PixelIterator PixelIterator;
        typedef ImageView::ConstPixelIterator ConstPixelIterator;

    public:
        Image();

        explicit Image(const ImageFormat& format);

        Image(const ImageFormat& format, const Size& size,
              size_t padding = 0);

        Image(const ImageFormat& format, Pt::uint8_t* buffer,
              const Size& size, size_t padding = 0);

        Image(const Image& image);

        virtual ~Image();

        const Image& operator=(const Image& image);

        void reset(const ImageFormat& format,
                   const Size& size, Pt::ssize_t padding = 0);

        void reset(const ImageFormat& format, Pt::uint8_t* data,
                   const Size& size, Pt::ssize_t padding = 0);

        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return PixelIterator(view(), x, y); }

        PixelIterator begin()
        { return PixelIterator(view(), 0, 0); }

        PixelIterator end()
        { return PixelIterator(view(), 0, height()); }

        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(view(), x, y); }

        ConstPixelIterator begin() const
        { return ConstPixelIterator(view(), 0, 0); }

        ConstPixelIterator end() const
        { return ConstPixelIterator(view(), 0, height()); }

        /** @brief Returns a view on the image data.
        */
        const ImageView& view() const
        {
            return _view;
        }

        /** @brief Returns a view on the image data.
        */
        ImageView& view()
        {
            return _view;
        }

        /** @brief Returns the format of the image.
        */
        const ImageFormat& format() const
        {
            return _view.format();
        }

        /** @brief Returns the size of the image.
        */
        const Size& size() const
        {
            return _view.size();
        }

        Pt::ssize_t width() const
        {
            return _view.width();
        }

        Pt::ssize_t height() const
        {
            return _view.height();
        }

        Pt::ssize_t padding() const
        {
            return _view.padding();
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

        void clear()
        {
            _buffer.clear();
            _view.clear();
        }

    private:
        std::vector<Pt::uint8_t> _buffer;
        ImageView                _view;
};

} // namespace

} // namespace

#endif
