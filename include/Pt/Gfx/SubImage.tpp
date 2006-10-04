/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#ifndef Pt_Gfx_SubImage_tpp
#define Pt_Gfx_SubImage_tpp


namespace Pt {

	namespace Gfx {

		//
		// IMPL: SubImage<ColorSpaceT>
		//
		template <typename ColorSpaceT_>
		SubImage<ColorSpaceT_>::SubImage(ImageT& image, const Rect& area)
		: _image(image), _area(area)
		{
			// Validate the area
			int x1 = area.x1();
			int y1 = area.y1();
			int x2 = x1 + area.width() - 1;
			int y2 = y1 + area.height() - 1;

			if(x1<0 || y1<0 || x2<0 ||y2<0 ||
			   x1>=int(_image.width()) || y1>=int(_image.height()) ||
			   x2>=int(_image.width()) || y2>=int(_image.height()))
				throw RangeError("The given rectangle covers invalid area in the given image ", PT_SOURCEINFO);
		}


		template <typename ColorSpaceT_>
		SubImage<ColorSpaceT_>& SubImage<ColorSpaceT_>::operator=(const typename SubImage<ColorSpaceT_>::ColorT& color)
		{
			for(uint y = _area.y1(); y < (_area.y1()+_area.height()); y++) {
				for(uint x = _area.x1(); x < (_area.x1()+_area.width()); x++) {
					_image.scanline(y)[x] = color;
				}
			}
			return *this;
		}


		template <typename ColorSpaceT_>
		template <typename SrcColorSpaceT>
		void SubImage<ColorSpaceT_>::_assign(const BasicImage<SrcColorSpaceT>& src)
		{
			// If the size is not the same, we need to scale convert first then copy
			if(_area.width()!=src.width() || _area.height()!=src.height()) {
				BasicImage<ColorSpaceT_> tmp;
				cubicScale(tmp, src, _area.width(), _area.height());
				for(uint y = 0; y < _area.height(); y++)
					for(uint x = 0; x < _area.width(); x++)
						_image[y+_area.y1()][x+_area.x1()] = tmp[y][x];
			}
			// If the size is the same, we just need to copy convert
			else {
				for(uint y = 0; y < _area.height(); y++)
					for(uint x = 0; x < _area.width(); x++)
						convert(_image[y+_area.y1()][x+_area.x1()], src[y][x]);
			}
		}


		template <typename ColorSpaceT_>
		template <typename SrcColorSpaceT>
		void SubImage<ColorSpaceT_>::_assign(SubImage<SrcColorSpaceT>& src)
		{
			// If the size is not the same, we need to convert scale first then copy
			if(_area.width()!=src.width() || _area.height()!=src.height()) {
				BasicImage<SrcColorSpaceT> tmp(src.width(), src.height());
				// Copy convert the source pixels to the temporary image
				for(uint y = 0; y < src.height(); y++)
					for(uint x = 0; x < src.width(); x++)
						convert(tmp[y][x], src._image[y+src._area.y1()][x+src._area.x1()]);
				// Scale the temporary image
				cubicScale(tmp, _area.width(), _area.height());
				// Copy the pixels to this sub image
				for(uint y = 0; y < _area.height(); y++)
					for(uint x = 0; x < _area.width(); x++)
						_image[y+_area.y1()][x+_area.x1()] = tmp[y][x];
			}
			// If the size is the same, we just need to copy convert
			else {
				// If the source and destination image are the same
				// we must use temporary image
				if(&_image == &src._image) {
					BasicImage<SrcColorSpaceT> tmp(_area.width(), _area.height());
					// Copy convert the source pixels to the temporary image
					for(uint y = 0; y < src.height(); y++)
						for(uint x = 0; x < src.width(); x++)
							convert(tmp[y][x], src._image[y+src._area.y1()][x+src._area.x1()]);
					// Copy the pixels to this sub image
					for(uint y = 0; y < _area.height(); y++)
						for(uint x = 0; x < _area.width(); x++)
							_image[y+_area.y1()][x+_area.x1()] = tmp[y][x];
				}
				// If the source and destination image are different, just copy convert
				else {
					for(uint y = 0; y < _area.height(); y++)
						for(uint x = 0; x < _area.width(); x++)
							convert(_image[y+_area.y1()][x+_area.x1()], src._image[y+src._area.y1()][x+src._area.x1()]);
				}
			}
		}


		template <typename ColorSpaceT_>
		typename SubImage<ColorSpaceT_>::ColorT& SubImage<ColorSpaceT_>::at(int x, int y)
		{
			if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height())) RangeError("Either the image is empty or the (y,x) coordinate is invalid", PT_SOURCEINFO);
			return _image.data()[(y+_area.y1())*_image.width() + x+_area.x1()];
		}


		template <typename ColorSpaceT_>
		const typename SubImage<ColorSpaceT_>::ColorT& SubImage<ColorSpaceT_>::at(int x, int y) const
		{
			if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height())) RangeError("Either the image is empty or the (y,x) coordinate is invalid", PT_SOURCEINFO);
			return _image.data()[(y+_area.y1())*_image.width() + x+_area.x1()];
		}


		template <typename ColorSpaceT_>
		const typename SubImage<ColorSpaceT_>::ColorT& SubImage<ColorSpaceT_>::color(int x, int y, const ColorT& invalid) const
		{
			if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height())) return invalid;
			return _image.data()[(y+_area.y1())*_image.width() + x+_area.x1()];
		}


		template <typename ColorSpaceT_>
		void SubImage<ColorSpaceT_>::setColor(int x, int y, const ColorT& color_)
		{
			if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height())) return;
			_image.data()[(y+_area.y1())*_image.width() + x+_area.x1()] = color_;
		}

	} // namespace Gfx

} // namespace Pt

#endif
