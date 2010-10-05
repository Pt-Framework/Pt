/*
 * Copyright (C) 2007 by Aloysius Indrayanto
 * Copyright (C) 2007 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Gfx_PlanarImageModel_h
#define Pt_Gfx_PlanarImageModel_h

#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/GenericAlgo.h>

namespace Pt {

    namespace Gfx {

        template <typename ComponentT, size_t NumberOfChannelsT>
        class PlanarConstColorRef
        {
            public:
                static const size_t NumberOfChannels = NumberOfChannelsT;

                typedef ComponentT Component;

                typedef const Component* ConstColorData [ NumberOfChannels ];

            public:
                inline PlanarConstColorRef(const PlanarConstColorRef& c)
                { assignElements<NumberOfChannels, 0>(_data, c._data); }

                inline PlanarConstColorRef(ConstColorData& data)
                { assignElements<NumberOfChannels, 0>(_data, data); }

                const ConstColorData& data() const
                { return _data; }

            protected:
                ConstColorData _data;
        };


        template <typename ComponentT, size_t NumberOfChannelsT>
        class PlanarColorRef
        {
            public:
                static const size_t NumberOfChannels = NumberOfChannelsT;

                typedef ComponentT Component;

                typedef PlanarConstColorRef<Component, NumberOfChannels> ConstColorRef;

                typedef ComponentT* ColorData [ NumberOfChannels ];

            public:
                inline PlanarColorRef(const PlanarColorRef& c)
                { assignElements<NumberOfChannels, 0>(_data, c._data); }

                inline PlanarColorRef(ColorData& c)
                { assignElements<NumberOfChannels, 0>(_data, c); }

                PlanarColorRef& operator=(const PlanarColorRef& other)
                {
                    // TODO: helper function to avoid for loop
                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        *_data[i] = *other._data[i];

                    return *this;
                }

                PlanarColorRef& operator=(const ConstColorRef& other)
                {
                    // TODO: helper function to avoid for loop
                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        *_data[i] = *other.data()[i];

                    return *this;
                }

            protected:
                ColorData _data;
        };


        template <typename ComponentT, size_t NumChannels>
        class PlanarColorPtr
        {
            public:
                static const size_t NumberOfChannels = NumChannels;

                typedef ComponentT* ColorData [ NumberOfChannels ];

                typedef PlanarColorRef<ComponentT, NumberOfChannels> ColorRef;

                PlanarColorPtr()
                {
                    // TODO: helper function to avoid for loop
                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        _data[i] = 0;
                }

                PlanarColorPtr(ColorData& data)
                {
                    assignElements<NumberOfChannels, 0>(_data, data);
                }

                PlanarColorPtr& operator=(const PlanarColorPtr& other)
                {
                    assignElements<NumberOfChannels, 0>(_data, other._data);
                    return *this;
                }

                ComponentT*& operator[](size_t n)
                { return _data[n]; }

                ComponentT* operator[](size_t n) const
                { return _data[n]; }

                bool operator!=(const PlanarColorPtr& it) const
                { return this->_data[0] != it._data[0]; }

                ColorRef operator*()
                { return ColorRef(_data); }

                size_t operator-(const PlanarColorPtr& other) const
                { return _data[0] - other._data[0] ; }

                ColorData& colorData()
                { return _data; }

                const ColorData& colorData() const
                { return _data; }

            protected:
                ColorData _data;
        };


        template <typename ComponentT, size_t NumChannels>
        class PlanarConstColorPtr
        {
            public:
                static const size_t NumberOfChannels = NumChannels;

                typedef const ComponentT* ConstColorData [ NumberOfChannels ];

                typedef PlanarColorPtr<ComponentT, NumberOfChannels> ColorPtr;

                typedef PlanarConstColorRef<ComponentT, NumberOfChannels> ColorRef;

            public:
                PlanarConstColorPtr()
                {
                    // TODO: helper function to avoid for loop
                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        _data[i] = 0;
                }

                PlanarConstColorPtr(const ColorPtr& data)
                {
                    assignElements<NumberOfChannels, 0>( _data, data.colorData() );
                }

                PlanarConstColorPtr(ConstColorData& data)
                {
                    assignElements<NumberOfChannels, 0>(_data, data);
                }

                PlanarConstColorPtr& operator=(const PlanarConstColorPtr& other)
                {
                    assignElements<NumberOfChannels, 0>(_data, other._data);
                    return *this;
                }

                bool operator!=(const PlanarConstColorPtr& it) const
                { return this->_data[0] != it._data[0]; }

                ColorRef operator*()
                { return ColorRef(_data); }

                size_t operator-(const PlanarConstColorPtr& other) const
                { return _data[0] - other._data[0] ; }

                const ComponentT*& operator[](size_t n)
                { return _data[n]; }

                const ComponentT* operator[](size_t n) const
                { return _data[n]; }

                ConstColorData& colorData()
                { return _data; }

                const ConstColorData& colorData() const
                { return _data; }

            protected:
                ConstColorData _data;
        };


        template <typename ColorPtrT, typename ViewT>
        void set(ColorPtrT& color, ViewT& view, size_t xpos, size_t ypos)
        {
            const size_t planeOffset = xpos + ( ypos * view.width() );
            const size_t subsampleOffset = (xpos/ViewT::SubX) + (ypos/ViewT::SubY * view.width()/ViewT::SubY);

            color[0] = view.data() + planeOffset;
            addElements<ViewT::NumberOfChannels, 1>(color.colorData(), view.colorData(), subsampleOffset);
        }


        template <typename ColorPtrT, typename ViewT>
        void increment(ColorPtrT& color, ViewT& view, size_t xpos, size_t ypos)
        {
            ++color.colorData()[0];
            const size_t subsampleOffset = (xpos/ViewT::SubX) + (ypos/ViewT::SubY * view.width()/ViewT::SubY);
            addElements<ViewT::NumberOfChannels, 1>(color.colorData(), view.colorData(), subsampleOffset);
        }


        template <typename ColorPtrT, typename ViewT>
        void advance(ColorPtrT& color, size_t n, ViewT& view, size_t xpos, size_t ypos)
        {
            color.colorData()[0] += n;
            const size_t subsampleOffset = (xpos/ViewT::SubX) + (ypos/ViewT::SubY * view.width()/ViewT::SubY);
            addElements<ViewT::NumberOfChannels, 1>(color.colorData(), view.colorData(), subsampleOffset);
        }


        template <typename ColorModelT, size_t SubX, size_t SubY>
        class PlanarImageView;

        //template <typename ColorModelT, size_t SubX, size_t SubY>
        template <typename ViewT, typename ColorPtrT, typename ColorRefT>
        class PlanarPixelIterator
        {
            public:
                typedef ColorRefT ColorRef;
                typedef ColorPtrT ColorPtr;

            public:
                inline PlanarPixelIterator()
                : _view(0),_xpos(0), _ypos(0)
                {}

                PlanarPixelIterator(ViewT& view, size_t xpos, size_t ypos)
                : _view(&view)
                , _xpos(xpos)
                , _ypos(ypos)
                {
                    set( _color, view, _xpos, _ypos );
                }

                ColorRef operator*()
                { return *_color; }

                bool operator!=(const PlanarPixelIterator& it) const
                { return this->_color != it._color; }

                PlanarPixelIterator& operator++()
                {
                    if(++_xpos == _view->width() )
                    {
                        _xpos = 0;
                        ++_ypos;
                    }

                    increment(_color, *_view, _xpos, _ypos);
                    return *this;
                }

                PlanarPixelIterator& operator+=(size_t n)
                {
                    _ypos += n / _view->width();
                    _xpos += n % _view->width();

                    if(_xpos >= _view->width() )
                    {
                        _xpos -= _view->width();
                        ++_ypos;
                    }

                    advance(_color, n, *_view, _xpos, _ypos);
                    return *this;
                }

                PlanarPixelIterator& operator=(const PlanarPixelIterator& other)
                {
                    _view = other._view;
                    _xpos  = other._xpos;
                    _ypos  = other._ypos;
                    _color = other._color;
                    return *this;
                }

                inline Gfx::Size operator-(const PlanarPixelIterator& other) const
                {
                    const size_t pos    = _color - _view->colorData();
                    const size_t width  = pos / _view->height();
                    const size_t height = pos / _view->width();

                    const size_t otherPos    = other._color - other._view->colorData();
                    const size_t otherWidth  = otherPos / other._view->height();
                    const size_t otherHeight = otherPos / other._view->width();

                    return Gfx::Size(width - otherWidth, height - otherHeight);
                }

            private:
                ViewT*     _view;
                ColorPtr   _color;
                size_t     _xpos;
                size_t     _ypos;
        };


        /** @brief View of planar images.
        */
        template <typename ColorModelT, size_t SubX_, size_t SubY_>
        class PlanarImageView
        {
            public:
                static const size_t NumberOfChannels = ColorModelT::NumberOfChannels;
                static const size_t SubX = SubX_;
                static const size_t SubY = SubY_;

                typedef typename ColorModelT::Color Color;

                typedef typename ColorModelT::ConstColor ConstColor;

                typedef typename ColorModelT::ColorRef ColorRef;

                typedef typename ColorModelT::ConstColorRef ConstColorRef;

                typedef typename ColorModelT::ColorPtr ColorPtr;

                typedef typename ColorModelT::ConstColorPtr ConstColorPtr;

                typedef typename ColorModelT::Component Component;

                typedef PlanarImageView<ColorModelT, SubX, SubY> ImageView;

                typedef PlanarPixelIterator< ImageView, ColorPtr, ColorRef> PixelIterator;

                typedef PlanarPixelIterator< const ImageView, ConstColorPtr,ConstColorRef> ConstPixelIterator;

            public:
                PlanarImageView()
                : _width(0), _height(0)
                { }

                Pt::size_t size(size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(Component);
                    const size_t imageSize = planeSize + ( (NumberOfChannels-1) * planeSize/(SubX*SubY));
                    return imageSize;
                }

                void init(unsigned char* memory, size_t width, size_t height)
                {
                    const size_t planeSize = width * height ;//* sizeof(Component);

                    _imageData.colorData()[0] = reinterpret_cast<Component*>(memory);
                    _imageData.colorData()[1] = _imageData.colorData()[0] + planeSize;

                    for(size_t i = 2; i < NumberOfChannels; ++i)
                        _imageData.colorData()[i] = _imageData.colorData()[i-1] + planeSize/(SubX*SubY);

                    _width  = width;
                    _height = height;
                }

                size_t width() const
                { return _width; }

                size_t height() const
                { return _height; }

                ColorPtr& colorData()
                { return _imageData; }

                ConstColorPtr colorData() const
                { return _imageData; }

                Component* data()
                { return _imageData[0]; }

                const Component* data() const
                { return _imageData[0]; }

            private:
                ColorPtr  _imageData;
                size_t    _width;
                size_t    _height;
        };

    } // namespace Gfx

} // namespace Pt

#endif
