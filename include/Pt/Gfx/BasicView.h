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

#ifndef PT_GFX_BASIC_VIEW_H
#define PT_GFX_BASIC_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

template <typename ModelT>
class BasicView
{
    public:
        typedef ModelT                      Model;
        typedef typename ModelT::Pixel      Pixel;
        typedef typename ModelT::ConstPixel ConstPixel;

        typedef Pt::ssize_t       pos_t;
        typedef BasicPoint<pos_t> Point;
        typedef BasicSize<pos_t>  Size;
        typedef BasicRect<pos_t>  Rect;

        class PixelIterator
        {
            public:
                PixelIterator(BasicView& view, Pt::ssize_t x, Pt::ssize_t y)
                : _pixel(view, x, y)
                { }

                PixelIterator(const PixelIterator& it)
                : _pixel(it._pixel)
                {}

                PixelIterator& operator=(const PixelIterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                bool operator!=(const PixelIterator& it) const
                { return _pixel != it._pixel; }

                bool operator==(const PixelIterator& it) const
                { return _pixel == it._pixel; }

                Pixel& operator*()
                { return _pixel; }

                Pixel* operator->()
                { return &_pixel; }

                PixelIterator& operator++()
                {
                    _pixel.advance();
                    return *this;
                }

                PixelIterator& operator+=(Pt::ssize_t n)
                {
                    _pixel.advance(n);
                    return *this;
                }

            private:
                Pixel _pixel;
        };

        class ConstPixelIterator
        {
            public:
                ConstPixelIterator(const BasicView& view, Pt::ssize_t x, Pt::ssize_t y)
                : _pixel(view, x, y)
                { }

                ConstPixelIterator(const ConstPixelIterator& it)
                : _pixel(it._pixel)
                {}

                ConstPixelIterator& operator=(const ConstPixelIterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                bool operator!=(const ConstPixelIterator& it) const
                { return _pixel != it._pixel; }

                bool operator==(const ConstPixelIterator& it) const
                { return _pixel == it._pixel; }

                const ConstPixel& operator*() const
                { return _pixel; }

                const ConstPixel* operator->() const
                { return &_pixel; }

                ConstPixelIterator& operator++()
                {
                    _pixel.advance();
                    return *this;
                }

                ConstPixelIterator& operator+=(Pt::ssize_t n)
                {
                    _pixel.advance(n);
                    return *this;
                }

            private:
                ConstPixel _pixel;
        };

    public:
        BasicView()
        : _model(0)
        , _data(0)
        , _width(0)
        , _height(0)
        , _padding(0)
        , _stride(0)
        {
            //_model = ModelT::best() 
        }

        BasicView(const ModelT& model)
        : _model(&model)
        , _data(0)
        , _width(0)
        , _height(0)
        , _padding(0)
        , _stride(0)
        { }

        BasicView(const ModelT& model, Pt::uint8_t* data, 
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        : _model(&model)
        , _data(data)
        , _width(width)
        , _height(height)
        , _padding(padding)
        , _stride(0)
        {
            _stride = (_width * model.pixelStride()) + _padding;
        }

        virtual ~BasicView()
        { }

        void reset(const ModelT& model, Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        {
            _model = &model;
            _data = data;
            _width = width,
            _height = height;
            _padding = padding;
            _stride = _width * model.pixelStride() + _padding;
        }

        void clear()
        {
            _data = 0;
            _width = 0;
            _height = 0;
            _padding = 0;
            _stride = 0;
        }

        /** @brief Returns an iterator to the pixel at the given position.
        */
        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return PixelIterator(*this, x, y); }

        /** @brief Returns an iterator to the first pixel.
        */
        PixelIterator begin()
        { return PixelIterator(*this, 0, 0); }

        /** @brief Returns an iterator to the end of the pixels.
        */
        PixelIterator end()
        { return PixelIterator(*this, 0, height()); }

        /** @brief Returns a const iterator to the pixel at the given position.
        */
        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(*this, x, y); }

        /** @brief Returns a const iterator to the first pixel.
        */
        ConstPixelIterator begin() const
        { return ConstPixelIterator(*this, 0, 0); }

        /** @brief Returns a const iterator to the end of the pixels.
        */
        ConstPixelIterator end() const
        { return ConstPixelIterator(*this, 0, height()); }

        Pt::ssize_t width() const
        { return _width; }

        Pt::ssize_t height() const
        { return _height; }

        bool empty() const
        { return _width == 0 || _height == 0; }

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        Pt::ssize_t stride() const
        { return _stride; }

        Pt::ssize_t padding() const
        { return _padding; }

        std::size_t pixelStride() const
        { return _model ? _model->pixelStride() : 0; }

        const ModelT* model() const
        { return _model; }

    private:
        const ModelT* _model;
        
        Pt::uint8_t*  _data;
        Pt::ssize_t   _width;
        Pt::ssize_t   _height;
        Pt::ssize_t   _padding;
        Pt::ssize_t   _stride;
};

} // namespace

} // namespace

#endif
