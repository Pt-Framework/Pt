/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Gfx_Yv12Image_h
#define Pt_Gfx_Yv12Image_h

#include <Pt/Gfx/PlanarImage.h>


namespace Pt {

    namespace Gfx {

        struct Yuv {};


        struct PlanarYuv : public Yuv {};


        template <>
        class Color<Yuv>
        {
            public:
                static const size_t NumberOfChannels = 3;

                typedef uint8_t ComponentT;

                typedef Color<Yuv> ValueT;

            public:
                inline Color(const Color& c)
                : _y(c._y), _u(c._u), _v(c._v)
                {}

                inline Color(ComponentT y, ComponentT u, ComponentT v)
                : _y(y), _u(u), _v(v)
                {}

                inline ComponentT y() const
                { return _y; }

                inline ComponentT u() const
                { return _u; }

                inline ComponentT v() const
                { return _v; }

                inline void setY(ComponentT y)
                { _y = y; }

                inline void setU(ComponentT u)
                { _u = u; }

                inline void setV(ComponentT v)
                { _v = v; }

            protected:
                ComponentT _y, _u, _v;
        };

        typedef Color<Yuv> YuvColor;


        template <>
        class Color<PlanarYuv>
        {
            public:
                static const size_t NumberOfChannels = 3;

                typedef uint8_t ComponentT;

            public:
                inline Color(const Color& c)
                : _y(c._y), _u(c._u), _v(c._v)
                {}

                inline Color(ComponentT &y, ComponentT &u, ComponentT &v)
                : _y(y), _u(u), _v(v)
                {}

                inline ComponentT y() const
                { return _y; }

                inline ComponentT u() const
                { return _u; }

                inline ComponentT v() const
                { return _v; }

                inline void setY(ComponentT y)
                { _y = y; }

                inline void setU(ComponentT u)
                { _u = u; }

                inline void setV(ComponentT v)
                { _v = v; }

            protected:
                ComponentT &_y, &_u, &_v;
        };

        typedef Color<PlanarYuv> Yv12Color;


        template<typename ModelT, typename ColorT, typename ComponentT>
        class Yv12PixelIterator;

        class Yv12Model
        {
            public:
                typedef uint8_t Component;

                typedef YuvColor ValueT;

                typedef Yv12Color Color;

                typedef YuvColor ConstColor;

                typedef Yv12PixelIterator< Yv12Model, Color, Component> PixelIterator;

                typedef Yv12PixelIterator< const Yv12Model, ConstColor, const Component> ConstPixelIterator;

                typedef PixelIterator ColorPtrT;

                static const size_t NumberOfChannels = 3;

                Yv12Model()
                : _ydata(0), _udata(0), _vdata(0), _width(0), _height(0)
                {}

                Pt::size_t size(size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(Component);
                    const size_t imageSize = planeSize + (2 * planeSize/4);
                    return imageSize;
                }

                void init(unsigned char* memory, size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(Component);

                    _ydata = reinterpret_cast<Component*>(memory);
                    _udata = _ydata + planeSize;
                    _vdata =  _udata + planeSize/4;

                    _width  = width;
                    _height = height;
                }

                size_t width() const
                { return _width; }

                size_t height() const
                { return _height; }

                Component* data()
                { return _ydata; }

                const Component* data() const
                { return _ydata; }

                Component* udata()
                { return _udata; }

                const Component* udata() const
                { return _udata; }

                Component* vdata()
                { return _vdata; }

                const Component* vdata() const
                { return _vdata; }

            private:
                Component* _ydata;
                Component* _udata;
                Component* _vdata;
                size_t _width;
                size_t _height;
        };

        typedef PlanarImage<Yv12Model> Yv12Image;


        template <typename ModelT, typename ColorT, typename ComponentT>
        class Yv12PixelIterator
        {
            public:
                inline Yv12PixelIterator()
                : _model(0), _y(0), _u(0), _v(0), _xpos(0), _ypos(0)
                {}

                Yv12PixelIterator(ModelT& model, size_t xpos, size_t ypos)
                : _model(&model)
                , _xpos(xpos)
                , _ypos(ypos)
                {
                    const size_t channelOffset = _xpos + ( _ypos * _model->width() );
                    const size_t subchannelOffset = ( _xpos/2 ) + ( _ypos/2 * _model->width()/2 );

                    _y = _model->data() + channelOffset;
                    _u = _model->udata() + subchannelOffset;
                    _v = _model->vdata() + subchannelOffset;
                }

                ColorT operator*()
                { return ColorT( *_y, *_u, *_v); }

                bool operator!=(const Yv12PixelIterator& it) const
                { return this->_y != it._y; }

                Yv12PixelIterator& operator++()
                {
                    ++_y;

                    if(++_xpos == _model->width() )
                    {
                        _xpos = 0;
                        ++_ypos;
                    }

                    const size_t subchannelOffset = ((_xpos/2) + (_ypos/2 * _model->width()/2));

                    _u = _model->udata() + subchannelOffset;
                    _v = _model->vdata() + subchannelOffset;

                    return *this;
                }

                Yv12PixelIterator operator=(const Yv12PixelIterator& other)
                {
                    _model = other._model;
                    _y     = other._y;
                    _u     = other._u;
                    _v     = other._v;
                    _xpos  = other._xpos;
                    _ypos  = other._ypos;
                    return *this;
                }

                inline Math::Size operator-(const Yv12PixelIterator& other) const
                {
                    const size_t pos    = _y - _model->data();
                    const size_t width  = pos / _model->height();
                    const size_t height = pos / _model->width();

                    const size_t otherPos    = other._y - other._model->data();
                    const size_t otherWidth  = otherPos / other._model->height();
                    const size_t otherHeight = otherPos / other._model->width();

                    return Math::Size(width - otherWidth, height - otherHeight);
                }

            private:
                ModelT* _model;
                ComponentT* _y;
                ComponentT* _u;
                ComponentT* _v;
                size_t _xpos;
                size_t _ypos;
        };


        template <typename ModelT, typename ColorT, typename ComponentT>
        class PlanarPixelIterator2x2
        {
            public:
                typedef ComponentT* Components[ ColorT::NumberOfChannels ];

                inline PlanarPixelIterator2x2()
                : _model(0),_xpos(0), _ypos(0)
                {}

                PlanarPixelIterator2x2(ModelT& model, size_t xpos, size_t ypos)
                : _model(&model)
                , _xpos(xpos)
                , _ypos(ypos)
                {
                    //const size_t channelOffset = _xpos + ( _ypos * _model->width() );
                    //const size_t subchannelOffset = ( _xpos/2 ) + ( _ypos/2 * _model->width()/2 );

                    //_y = _model->data() + channelOffset;
                    //_u = _model->udata() + subchannelOffset;
                    //_v = _model->vdata() + subchannelOffset;
                }

            private:
                ModelT*    _model;
                Components _data;
                size_t     _xpos;
                size_t     _ypos;
        };
    }

}

#endif
