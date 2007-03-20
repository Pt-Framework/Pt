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

                typedef Color<Yuv> ValueT;

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

        typedef Color<PlanarYuv> Yv12ColorRef;


        class Yv12Model;


        class Yv12ColorPtr
        {
            public:
                typedef YuvColor::ComponentT ComponentT;
                typedef Yv12ColorRef ColorRef;

            public:
                Yv12ColorPtr(Yv12Model& model, size_t xpos, size_t ypos);

                ColorRef operator*()
                { return ColorRef( *_y, *_u, *_v); }

                Yv12ColorPtr& operator++();

            private:
                Yv12Model* _model;
                ComponentT* _y;
                ComponentT* _u;
                ComponentT* _v;
                size_t _xpos;
                size_t _ypos;
        };


        class Yv12Model
        {
            public:
                typedef uint8_t ComponentT;

                typedef Color<Yuv> ValueT;

                typedef Yv12ColorPtr ColorPtrT;

                static const size_t NumberOfChannels = 3;

                Yv12Model()
                : _ydata(0), _udata(0), _vdata(0), _width(0), _height(0)
                {}

                Pt::size_t size(size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(ComponentT);
                    const size_t imageSize = planeSize + (2 * planeSize/4);
                    return imageSize;
                }

                void init(unsigned char* memory, size_t width, size_t height)
                {
                    const size_t planeSize = width * height * sizeof(ComponentT);

                    _ydata = reinterpret_cast<ComponentT*>(memory);
                    _udata = _ydata + planeSize;
                    _vdata =  _udata + planeSize/4;
                    //for(size_t i = 1; i < NumberOfChannels; ++i)
                    //    _chanPtr[i] = _chanPtr[i-1] + planeSize;

                    _width  = width;
                    _height = height;
                }

                size_t width() const
                { return _width; }

                size_t height() const
                { return _height; }

                ComponentT* data()
                { return _ydata; }

                ComponentT* udata()
                { return _udata; }

                ComponentT* vdata()
                { return _vdata; }

            private:
                ComponentT* _ydata;
                ComponentT* _udata;
                ComponentT* _vdata;
                size_t _width;
                size_t _height;
        };

        typedef PlanarImage<Yv12Model> Yv12Image;


        inline Yv12ColorPtr::Yv12ColorPtr(Yv12Model& model, size_t xpos, size_t ypos)
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


        inline Yv12ColorPtr& Yv12ColorPtr::operator++()
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

    }

}

#endif
