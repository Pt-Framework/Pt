/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx2_SubImage_h
#define Pt_Gfx2_SubImage_h

#include <Pt/Gfx/Region.h>
#include <Pt/Gfx2/ImageAlgo.h>


namespace Pt {

	namespace Gfx {

		/** @brief Sub image class.
		 *  @ingroup Gfx
		 */
		template <typename ImageT_>
		class SubImage {
			public:
				typedef typename ImageT_::ColorT ColorT;
				typedef ImageT_                  ImageT;
				typedef SubImage<ImageT_>        SubImageT;

				typedef ColorT*       Scanline;
				typedef const ColorT* ConstScanline;

			protected:
				class PixelIterator;
				class ConstPixelIterator;

			public:
				//! Construct a subimage using the given image and area.
				SubImage(ImageT& image, const Pt::Gfx::Region& area);


				//! Comparison based on the pixels' color values.
				bool operator==(const SubImageT& src);


				//! Check if the image is empty or not
				inline bool empty() const
				{ return _image.empty(); }


				//! Return the area of the subimage (in the term of the main/full image)
				inline const Pt::Gfx::Region& region() const
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


				//! Fill the sub image with the given color.
				SubImage& operator=(const ColorT& color)
				{
					for(size_t y = 0; y < _area.height(); y++) {
						for(size_t x = 0; x < _area.width(); x++) {
							_image.scanline(y)[x] = color;
						}
					}
					return *this;
				}

				//! Fill the sub image with the given image.
				SubImageT& operator=(const ImageT& src);

				//! Fill the sub image with the sub given image.
				SubImageT& operator=(SubImageT& src);

				//! Scanline access without range check
				inline Scanline scanline(int y)
				{ return &_image.data()[(y+_area.y())*_image.width() + _area.x()]; }

				//! Scanline access without range check
				inline ConstScanline scanline(int y) const
				{ return &_image.data()[(y+_area.y())*_image.width() + _area.x()]; }

				//! Random access without range check
				inline ColorT& pixel(int x, int y)
				{ return _image.data()[(y+_area.y())*_image.width() + x+_area.x()]; }

				//! Random access without range check
				inline const ColorT& pixel(int x, int y) const
				{ return _image.data()[(y+_area.y())*_image.width() + x+_area.x()]; }

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
				{ return PixelIterator(*this, 1, _area.height() + 1); }

				//! Returns an iterator indicating the position
				//! of a pixel at(x,y)
				inline PixelIterator iterator(uint y, uint x)
				{ return PixelIterator(*this, _area.x()+y, _area.y()+x); }

				//! Returns a const iterator indicating the position
				//! of the first pixel in this image
				inline ConstPixelIterator begin() const
				{ return ConstPixelIterator(*this); }

				//! Returns a const iterator indicating the position
				//! after the last pixel in this image
				inline ConstPixelIterator end() const
				{ return ConstPixelIterator(*this, 1, _area.height() + 1 ); }

				//! Returns a const iterator indicating the position
				//! of a pixel at(x,y)
				inline ConstPixelIterator iterator(uint y, uint x) const
				{ return ConstPixelIterator(*this, _area.x()+y, _area.y()+x); }

			protected:
				ImageT& _image;
				Region  _area;

			protected:
				/** @brief Pixel-based iterator class.
				  * @ingroup Gfx
				  */
				class PixelIterator {
					public:
						typedef typename ImageT_::ColorT ColorT;
						typedef ImageT_                  ImageT;

					public:
						inline PixelIterator(SubImageT& image)
						: _pixel(&image.scanline(0)[0]),
						  _offsetX(0), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1)
						{}

						inline PixelIterator(SubImageT& image, uint x, uint y)
						: _pixel(&image.scanline(y-1)[x-1]),
						  _offsetX(x), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1)
						{}

						inline bool operator==(const PixelIterator& it) const
						{ return _pixel == it._pixel; }

						inline bool operator!=(const PixelIterator& it) const
						{ return _pixel != it._pixel; }

						inline ColorT& operator*() const
						{ return *_pixel; }

						inline PixelIterator& operator++()
						{
							if(++_currentX == _width ) { // At the end of line
								_currentX = 0;
								_pixel   += _incr;
								return *this;
							}
							++_pixel;
							return *this;
						}

					private:
						ColorT* _pixel;
						uint    _offsetX;
						uint    _currentX;
						uint    _width;
						uint    _incr;
				};

				/** @brief Pixel-based constant iterator class.
				  * @ingroup Gfx
				  */
				class ConstPixelIterator {
					public:
						typedef typename ImageT_::ColorT ColorT;
						typedef ImageT_                  ImageT;

					public:
						inline ConstPixelIterator(const SubImageT& image)
						: _pixel(&image.scanline(0)[0]),
						  _offsetX(0), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1)
						{
						}

						inline ConstPixelIterator(const SubImageT& image, uint x, uint y)
						: _pixel(&image.scanline(y-1)[x-1]),
						  _offsetX(x), _currentX(0), _width(image.width()),
						  _incr(image.fullImage().width() - image.width() + 1)
						{
						}

						inline bool operator==(const ConstPixelIterator& it) const
						{ return _pixel == it._pixel; }

						inline bool operator!=(const ConstPixelIterator& it) const
						{ return _pixel != it._pixel; }

						inline const ColorT& operator*() const
						{ return *_pixel; }

						inline ConstPixelIterator& operator++()
						{
							if(++_currentX == _width) { // At the end of line
								_currentX = 0;
								_pixel   += _incr;
								return *this;
							}
							++_pixel;
							return *this;
						}

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
