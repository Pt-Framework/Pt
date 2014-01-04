/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Duerner                          *
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
#ifndef Pt_Gfx_InterleavedImage_tpp
#define Pt_Gfx_InterleavedImage_tpp


namespace Pt {

    namespace Gfx {

        template <typename ColorT_>
        void InterleavedImage<ColorT_>::resize(uint width_, uint height_)
        {
            _buff.resize(width_ * height_);
            _width  = width_;
            _height = height_;
        }

        template <typename ColorT_>
        void InterleavedImage<ColorT_>::resize(uint width_, uint height_, const ColorT& fill)
        {
            _buff.resize(width_ * height_);
            _width  = width_;
            _height = height_;

            for(uint i = 0; i < _width*_height; i++) _buff[i] = fill;
        }


        template <typename ColorT_>
        InterleavedImage<ColorT_>& InterleavedImage<ColorT_>::operator=(const ColorT& fill)
        {
            for(uint i = 0; i < _width*_height; i++) _buff[i] = fill;
            return *this;
        }

        template <typename ColorT_>
        InterleavedImage<ColorT_>& InterleavedImage<ColorT_>::operator=(const InterleavedImage& src)
        {
            if(src.empty()) {
            clear();
            return *this;
            }

            if(_width!=src._width || _height!=src._height) {
            _buff.resize(src._width * src._height);
            _width  = src._width;
            _height = src._height;
            };
            std::memcpy(&_buff[0], &src._buff[0], _width * _height * sizeof(ColorT));

            return *this;
        }


        template <typename ColorT_>
        typename InterleavedImage<ColorT_>::ColorT& InterleavedImage<ColorT_>::at(int x, int y)
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height))
            throw std::range_error("Either the image is empty or the (y,x) coordinate is invalid" + PT_SOURCEINFO);

            return _buff[y*_width + x];
        }

        template <typename ColorT_>
        const typename InterleavedImage<ColorT_>::ColorT& InterleavedImage<ColorT_>::at(int x, int y) const
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height))
            throw std::range_error("Either the image is empty or the (y,x) coordinate is invalid" + PT_SOURCEINFO);

            return _buff[y*_width + x];
        }

        template <typename ColorT_>
        const typename InterleavedImage<ColorT_>::ColorT& InterleavedImage<ColorT_>::color(int x, int y, const ColorT& invalid) const
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height)) return invalid;

            return _buff[y*_width + x];
        }

        template <typename ColorT_>
        void InterleavedImage<ColorT_>::setColor(int x, int y, const ColorT& color_)
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height)) return;

            _buff[y*_width + x] = color_;
        }

    } // namespace Gfx

} // namespace Pt

#endif

