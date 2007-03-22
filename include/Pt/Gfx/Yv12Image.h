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
#include <Pt/Gfx/GenericAlgo.h>


namespace Pt {

    namespace Gfx {

        struct Yuv {};


        template <>
        class Color<Yuv>
        {
            public:
                static const size_t NumberOfChannels = 3;

                typedef uint8_t ComponentT;

                typedef ComponentT* ColorData [ NumberOfChannels ];

                typedef const ComponentT* ConstColorData [ NumberOfChannels ];

            public:
                inline Color(const Color& c)
                : _y(c._y), _u(c._u), _v(c._v)
                {}

                inline Color(ColorData c)
                : _y(*c[0]), _u(*c[1]), _v(*c[2])
                {}

                inline Color(ConstColorData c)
                : _y(*c[0]), _u(*c[1]), _v(*c[2])
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


        struct PlanarYuv : public Yuv {};


        template <>
        class Color<PlanarYuv>
        {
            public:
                static const size_t NumberOfChannels = 3;

                typedef uint8_t ComponentT;

                typedef ComponentT* ColorData [ NumberOfChannels ];

                typedef const ComponentT* ConstColorData [ NumberOfChannels ];

                typedef YuvColor ConstColor;

            public:
                inline Color(const Color& c)
                : _y( c._y ), _u( c._u ), _v( c._v )
                {}

                inline Color(ColorData c)
                : _y(c[0]), _u(c[1]), _v(c[2])
                {}

                inline Color(ComponentT &y, ComponentT &u, ComponentT &v)
                : _y(&y), _u(&u), _v(&v)
                {}

                inline ComponentT y() const
                { return *_y; }

                inline ComponentT u() const
                { return *_u; }

                inline ComponentT v() const
                { return *_v; }

                inline void setY(ComponentT y)
                { *_y = y; }

                inline void setU(ComponentT u)
                { *_u = u; }

                inline void setV(ComponentT v)
                { *_v = v; }

            protected:
                ComponentT* _y;
                ComponentT* _u;
                ComponentT* _v;
        };

        typedef Color<PlanarYuv> Yv12Color;

/*
        struct Yv12 : public PlanarYuv {};


        template <>
        struct ColorTraits<Yv12>
        {
        };
*/

        template<typename ModelT, typename ColorT, typename ComponentT>
        class PlanarPixelIterator2x2;


        template <typename ColorT>
        class PlanarImageModel<ColorT, 2, 2>
        {
            public:
                typedef ColorT Color;

                typedef typename Color::ComponentT Component;

                typedef typename Color::ConstColor ConstColor;

                typedef typename Color::ConstColor ValueT;

                typedef typename Color::ColorData ColorData;

                typedef typename Color::ConstColorData ConstColorData;

                typedef PlanarImageModel<ColorT, 2, 2> Model;

                typedef PlanarPixelIterator2x2< Model, Color, ColorData> PixelIterator;

                typedef PlanarPixelIterator2x2< const Model, ConstColor, ConstColorData> ConstPixelIterator;

                typedef PixelIterator ColorPtrT;

            public:
                PlanarImageModel()
                : _width(0), _height(0)
                {
                    for(unsigned n = 0; n < Color::NumberOfChannels; ++n)
                        _data[n] = 0;
                }

                Pt::size_t size(size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(Component);
                    const size_t imageSize = planeSize + (2 * planeSize/4);
                    return imageSize;
                }

                void init(unsigned char* memory, size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(Component);

                    _data[0] = reinterpret_cast<Component*>(memory);
                    _data[1] = _data[0] + planeSize;


                    for(size_t i = 2; i < Color::NumberOfChannels; ++i)
                        _data[i] = _data[i-1] + planeSize/4;

                    _width  = width;
                    _height = height;
                }

                size_t width() const
                { return _width; }

                size_t height() const
                { return _height; }

                ColorData& colorData()
                { return _data; }

                ConstColorData& colorData() const
                { return (ConstColorData&)_data; }

                Component* data()
                { return _data[0]; }

                const Component* data() const
                { return _data[0]; }

            private:
                ColorData _data;
                size_t    _width;
                size_t    _height;
        };

        typedef PlanarImageModel<Yv12Color, 2, 2> Yv12Model;

        typedef PlanarImage< Yv12Model > Yv12Image;


        template <typename ModelT, typename ColorT, typename DataT>
        class PlanarPixelIterator2x2
        {
            public:
                inline PlanarPixelIterator2x2()
                : _model(0),_xpos(0), _ypos(0)
                {}

                PlanarPixelIterator2x2(ModelT& model, size_t xpos, size_t ypos)
                : _model(&model)
                , _xpos(xpos)
                , _ypos(ypos)
                {
                    const size_t planeOffset = _xpos + ( _ypos * _model->width() );
                    const size_t subsampleOffset = ( _xpos/2 ) + ( _ypos/2 * _model->width()/2 );

                    _data[0] = _model->data() + planeOffset;
                    addElements<ColorT::NumberOfChannels, 1>(_data, _model->colorData(), subsampleOffset);
                }

                ColorT operator*()
                { return ColorT( _data); }

                bool operator!=(const PlanarPixelIterator2x2& it) const
                { return this->_data[0] != it._data[0]; }

                PlanarPixelIterator2x2& operator++()
                {
                    ++_data[0];

                    if(++_xpos == _model->width() )
                    {
                        _xpos = 0;
                        ++_ypos;
                    }

                    const size_t subsampleOffset = ((_xpos/2) + (_ypos/2 * _model->width()/2));
                    addElements<ColorT::NumberOfChannels, 1>(_data, _model->colorData(), subsampleOffset);
                    return *this;
                }

                PlanarPixelIterator2x2 operator=(const PlanarPixelIterator2x2& other)
                {
                    _model = other._model;
                    _xpos  = other._xpos;
                    _ypos  = other._ypos;
                    _data  = other._data;
                    return *this;
                }

                inline Math::Size operator-(const PlanarPixelIterator2x2& other) const
                {
                    const size_t pos    = _data[0] - _model->data();
                    const size_t width  = pos / _model->height();
                    const size_t height = pos / _model->width();

                    const size_t otherPos    = other._data[0] - other._model->data();
                    const size_t otherWidth  = otherPos / other._model->height();
                    const size_t otherHeight = otherPos / other._model->width();

                    return Math::Size(width - otherWidth, height - otherHeight);
                }

            private:
                ModelT*    _model;
                DataT      _data;
                size_t     _xpos;
                size_t     _ypos;
        };

    }

}

#endif



/** DEPRCATED
*/
/*
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
        ModelT*     _model;
        ComponentT* _y;
        ComponentT* _u;
        ComponentT* _v;
        size_t      _xpos;
        size_t      _ypos;
};
*/
