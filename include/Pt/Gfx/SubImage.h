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
#ifndef Pt_SubImage_h
#define Pt_SubImage_h

#include <Pt/Gfx/BasicImage.h>


namespace Pt {

	namespace Gfx {

		//! \brief SubImage class
		template <typename ColorSpaceT_>
		class PT_EXPORT SubImage {
			public:
				typedef BasicImage<ColorSpaceT_>     ImageT;
				typedef SubImage<ColorSpaceT_>       SubImageT;
				typedef typename ImageT::ColorSpaceT ColorSpaceT;
				typedef typename ImageT::ColorT      ColorT;

				typedef ColorT*       Scanline;
				typedef const ColorT* ConstScanline;

			public:
				class PixelIterator;
				class ConstPixelIterator;

			public:
				//! Construct a subimage using the given image and area
				SubImage(ImageT& image, const Rect& area);

				//! We can fill the sub image using a color
				SubImage& operator=(const ColorT& color);

				//! We can fill the sub image using any other images
				template <typename SrcColorSpaceT> inline
				SubImage<ColorSpaceT_>& operator=(const BasicImage<SrcColorSpaceT>& src)
				{ _assign(src); return *this; }

				//! We can fill the sub image using any other sub images
				template <typename SrcColorSpaceT> inline
				SubImage<ColorSpaceT_>& operator=(SubImage<SrcColorSpaceT>& src)
				{ _assign(src); return *this; }

				//! Check if the image is empty or not
				inline bool empty() const
				{ return _image.empty(); }

				//! Return the area of the subimage (in the term of the main image)
				inline const Rect& rect() const
				{ return _area; }

				//! Return the width of the subimage
				inline uint width () const
				{ return _area.width(); }

				//! Return the height of the subimage
				inline uint height() const
				{ return _area.height(); }

				//! Access to the full image
				inline ImageT& fullImage()
				{ return _image; }

				//! Access to the full image
				inline const ImageT& fullImage() const
				{ return _image; }

				//! Scanline access without range check
				inline Scanline scanline(int y)
				{ return &_image.data()[(y+_area.y1())*_image.width() + _area.x1()]; }

				//! Scanline access without range check
				inline ConstScanline scanline(int y) const
				{ return &_image.data()[(y+_area.y1())*_image.width() + _area.x1()]; }

				//! Random access without range check
				inline ColorT& pixel(int x, int y)
				{ return _image.data()[(y+_area.y1())*_image.width() + x+_area.x1()]; }

				//! Random access without range check
				inline const ColorT& pixel(int x, int y) const
				{ return _image.data()[(y+_area.y1())*_image.width() + x+_area.x1()]; }

				//! Random access with range check
				ColorT& at(int x, int y);

				//! Random access with range check
				const ColorT& at(int x, int y) const;

				//! Returns the color at the specified coordinates
				const ColorT& color(int x, int y, const ColorT& invalid = ColorT()) const;

				//! Returns the color at the specified coordinates
				void setColor(int x, int y, const ColorT& color_);

				//! Returns an iterator indicating the position
				//! of the first pixel in this image
				inline PixelIterator begin()
				{ return PixelIterator(*this); }
				//!
				//! Returns an iterator indicating the position
				//! after the last pixel in this image
				inline PixelIterator end()
				{ return PixelIterator(*this, _area.width(), _area.height()-1); }

				//! Returns an iterator indicating the position
				//! of a pixel at(x,y)
				inline PixelIterator iterator(uint y, uint x)
				{ return PixelIterator(*this, _area.x1()+y, _area.y1()+x); }

				//! Returns a const iterator indicating the position
				//! of the first pixel in this image
				inline ConstPixelIterator begin() const
				{ return ConstPixelIterator(*this); }

				//! Returns a const iterator indicating the position
				//! after the last pixel in this image
				inline ConstPixelIterator end() const
				{ return ConstPixelIterator(*this, _area.width(), _area.height()-1); }

				//! Returns a const iterator indicating the position
				//! of a pixel at(x,y)
				inline ConstPixelIterator iterator(uint y, uint x) const
				{ return ConstPixelIterator(*this, _area.x1()+y, _area.y1()+x); }

			protected:
				ImageT& _image;
				Rect    _area;

				// Helper fucntions
				template <typename SrcColorSpaceT>
					void _assign(const BasicImage<SrcColorSpaceT>& src);
				template <typename SrcColorSpaceT>
					void _assign(SubImage<SrcColorSpaceT>& src);

			public:
				//!
				//! \brief Pixel iterator class for SubImage<ColorSpaceT>
				//!
				class PixelIterator {
					public:
						typedef SubImage<ColorSpaceT_> SubImageT;

						typedef typename SubImageT::ColorSpaceT ColorSpaceT;
						typedef typename SubImageT::ColorT      ColorT;

					public:
						//! Construct a PixelIterator object at coordinate (0,0)
						inline PixelIterator(SubImageT& image)
						: _pixel(&image.scanline(0)[0]), _offsetX(0), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1) {}

						//! Construct a PixelIterator object at coordinate (x,y)
						inline PixelIterator(SubImageT& image, uint x, uint y)
						: _pixel(&image.scanline(y-1)[x-1]), _offsetX(x), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1) {}

						//! Return the pixel at the current coordinate
						inline ColorT& operator*() const
						{ return *_pixel; }

						//! Increment to the next pixel
						inline PixelIterator& operator++()
						{
							// At the end of line
							if(++_currentX == _width) {
								_currentX = 0;
								_pixel   += _incr;
								return *this;
							}

							++_pixel;
							return *this;
						}

						//! Unequality comparison operator
						inline bool operator!=(const PixelIterator& it) const
						{ return this->_pixel != it._pixel; }

					private:
						ColorT* _pixel;
						uint    _offsetX;
						uint    _currentX;
						uint    _width;
						uint    _incr;
				};

				//!
				//! \brief Constant pixel iterator class for SubImage<ColorSpaceT>
				//!
				class ConstPixelIterator {
					public:
						typedef SubImage<ColorSpaceT_> SubImageT;

						typedef typename SubImageT::ColorSpaceT ColorSpaceT;
						typedef typename SubImageT::ColorT      ColorT;

					public:
						//! Construct a ConstPixelIterator object at coordinate (0,0)
						inline ConstPixelIterator(const SubImageT& image)
						: _pixel(&image.scanline(0)[0]), _offsetX(0), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1) {}

						//! Construct a ConstPixelIterator object at coordinate (x,y)
						inline ConstPixelIterator(const SubImageT& image, uint x, uint y)
						: _pixel(&image.scanline(y-1)[x-1]), _offsetX(x), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1) {}

						//! Return the pixel at the current coordinate
						inline const ColorT& operator*() const
						{ return *_pixel; }

						//! Increment to the next pixel
						inline ConstPixelIterator& operator++()
						{
							// At the end of line
							if(++_currentX == _width) {
								_currentX = 0;
								_pixel   += _incr;
								return *this;
							}

							++_pixel;
							return *this;
						}
						//! Unequality comparison operator
						inline bool operator!=(const ConstPixelIterator& it) const
						{ return this->_pixel != it._pixel; }

					private:
						const ColorT* _pixel;
						uint          _offsetX;
						uint          _currentX;
						uint          _width;
						uint          _incr;
				};
		};

	} // namespace Gfx

} // namespace Pt

#endif

#include <Pt/Gfx/SubImage.tpp>
