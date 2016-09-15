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
#include <Pt/Gfx/Size.h>
#include <Pt/Types.h>
#include <vector>

namespace Pt {

namespace Gfx {

class PT_GFX_API ImageBase
{
    public:
        ImageBase();

        ImageBase(const ImageFormat& format, Pt::uint8_t* data, 
                  const Size& size, Pt::ssize_t padding);
    
        virtual ~ImageBase();

        const ImageView& view() const
        {
            return _view;
        }

        // TODO: image format must not be changed via BasicImage public API
        ImageView& view()
        {
            return _view;
        }

        const ImageFormat& format() const
        {
            return _view.format();
        }

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

    protected:
        void init(const ImageFormat& format, Pt::uint8_t* data, 
                  const Size& size, Pt::ssize_t padding);

    private:
        ImageView _view;
};


class PT_GFX_API Image : public ImageBase
{
    public:
        typedef ImageView::PixelIterator PixelIterator;
        typedef ImageView::ConstPixelIterator ConstPixelIterator;

    public:
        Image();
    
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
        { return view().pixel(x, y); }

        PixelIterator begin()
        { return view().begin(); }

        PixelIterator end()
        { return view().end(); }

        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return view().pixel(x, y); }

        ConstPixelIterator begin() const
        { return view().begin(); }

        ConstPixelIterator end() const
        { return view().end(); }

    private:
        std::vector<Pt::uint8_t> _buffer;
};


template <typename ModelT>
class BasicImage : public ImageBase
{
    typedef typename ModelT::PixelType      PixelType;
    typedef typename ModelT::ConstPixelType ConstPixelType;
    typedef typename ModelT::FormatType     FormatType;

    public:
        class PixelIterator
        {
            public:
                PixelIterator(ImageView& view, 
                              Pt::ssize_t xpos, Pt::ssize_t ypos)
                : _pixel(view, xpos, ypos)
                { }

                PixelIterator& operator=(const PixelIterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                PixelType& operator*()
                { return _pixel; }
                
                PixelType* operator->()
                { return &_pixel; }
                
                PixelIterator& operator++()
                {
                    _pixel.advance();
                    return *this; 
                }

                bool operator!=(const PixelIterator& it) const
                { return _pixel != it._pixel; }
        
                bool operator==(const PixelIterator& it) const
                { return _pixel == it._pixel; }

            private:
                PixelType _pixel;
        };

        class ConstPixelIterator
        {
            public:
                ConstPixelIterator(const ImageView& view, 
                                   Pt::ssize_t xpos, Pt::ssize_t ypos)
                : _pixel(view, xpos, ypos)
                { }

                ConstPixelIterator& operator=(const ConstPixelIterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                const ConstPixelType& operator*() const
                { return _pixel; }

                const ConstPixelType* operator->() const
                { return &_pixel; }

                ConstPixelIterator& operator++()
                {
                    _pixel.advance();
                    return *this; 
                }

                bool operator!=(const ConstPixelIterator& it) const
                { return _pixel != it._pixel; }
        
                bool operator==(const ConstPixelIterator& it) const
                { return _pixel == it._pixel; }

            private:
                ConstPixelType _pixel;
        };

    public:
        BasicImage(const Size& size, Pt::ssize_t padding = 0)
        : _buffer( _format.imageSize(size, padding) )
        { 
            Pt::uint8_t* data = _buffer.empty() ? 0 : &_buffer[0];
            init(_format, data, size, padding);
        }

        BasicImage(Pt::uint8_t* data, const Size& size, Pt::ssize_t padding = 0)
        { 
            init(_format, data, size, padding);
        }

        virtual ~BasicImage()
        {}

        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return PixelIterator(view(), x, y); }

        PixelIterator begin()
        { return PixelIterator(view(), 0, 0); }

        PixelIterator end()
        { return PixelIterator(view(), 0, height()); }

        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(view(), x, y); }

        ConstPixelIterator begin() const
        { return PixelIterator(view(), 0, 0); }

        ConstPixelIterator end() const
        { return PixelIterator(view(), 0, height()); }

    private:
        FormatType               _format;
        std::vector<Pt::uint8_t> _buffer;
};

} // namespace

} // namespace

#endif
