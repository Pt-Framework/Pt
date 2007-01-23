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
#ifndef Pt_Gfx2_SubImage_h
#define Pt_Gfx2_SubImage_h

#include <Pt/Gfx/Region.h>
#include <Pt/Gfx2/ImageAlgo.h>


namespace Pt {

	namespace Gfx {

		/** @brief Subimage class.
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

			public:
				class PixelIterator;
				class ConstPixelIterator;

			public:
				/** @brief Construct a subimage using the given image and area.
				 */
				SubImage(ImageT& image, const Pt::Gfx::Region& area);


				/** @brief Comparison based on the pixels' color values.
				 */
				bool operator==(const SubImageT& src);


				/** @brief Check if the image is empty or not.
				 */
				inline bool empty() const
				{ return _image.empty(); }


				/** @brief Return the area of the subimage (in the term of the main/full image).
				 */
				inline const Pt::Gfx::Region& region() const
				{ return _area; }

				/** @brief Return the width of the subimage.
				 */
				inline uint width () const
				{ return _area.width(); }

				/** @brief Return the height of the subimage.
				 */
				inline uint height() const
				{ return _area.height(); }

				/** @brief Access to the full image.
				 */
				inline ImageT& fullImage()
				{ return _image; }

				/** @brief Access to the full image.
				 */
				inline const ImageT& fullImage() const
				{ return _image; }


				/** @brief Fill the subimage with the given color.
				 */
				SubImage& operator=(const ColorT& color);

				/** @brief Fill the subimage with the given image.
				 */
				SubImageT& operator=(const ImageT& src);

				/** @brief Fill the subimage with the given subimage.
				 */
				SubImageT& operator=(const SubImageT& src);


				/** @brief Scanline access without range check.
				 */
				inline Scanline scanline(int y)
				{ return &_image.data()[(y+_area.y())*_image.width() + _area.x()]; }

				/** @brief Scanline access without range check.
				 */
				inline ConstScanline scanline(int y) const
				{ return &_image.data()[(y+_area.y())*_image.width() + _area.x()]; }

				/** @brief Random access without range check.
				 */
				inline ColorT& pixel(int x, int y)
				{ return _image.data()[(y+_area.y())*_image.width() + x+_area.x()]; }

				/** @brief Random access without range check.
				 */
				inline const ColorT& pixel(int x, int y) const
				{ return _image.data()[(y+_area.y())*_image.width() + x+_area.x()]; }

				/** @brief Random access with range check.
				 */
				ColorT& at(int x, int y);

				/** @brief Random access with range check.
				 */
				const ColorT& at(int x, int y) const;

				/** @brief Return the color at the specified coordinates.
				 */
				const ColorT& color(int x, int y, const ColorT& invalid = ColorT()) const;

				//!
				/** @brief Return the color at the specified coordinates.
				 */
				void setColor(int x, int y, const ColorT& color_);


				/** @brief Return an iterator indicating the position of the first pixel in this image.
				 */
				inline PixelIterator begin()
				{ return PixelIterator(*this); }

				/** @brief Return an iterator indicating the position after the last pixel in this image.
				 */
				inline PixelIterator end()
				{ return PixelIterator(*this, 1, _area.height() + 1); }

				/** @brief Return an iterator indicating the position of a pixel at(x,y).
				 */
				inline PixelIterator iterator(uint y, uint x)
				{ return PixelIterator(*this, _area.x()+y, _area.y()+x); }

				/** @brief Return a const antiterator indicating the position of the first pixel in this image.
				 */
				inline ConstPixelIterator begin() const
				{ return ConstPixelIterator(*this); }

				/** @brief Return a constant iterator indicating the position after the last pixel in this image.
				 */
				inline ConstPixelIterator end() const
				{ return ConstPixelIterator(*this, 1, _area.height() + 1 ); }

				/** @brief Return a constant iterator indicating the position of a pixel at(x,y).
				 */
				inline ConstPixelIterator iterator(uint y, uint x) const
				{ return ConstPixelIterator(*this, _area.x()+y, _area.y()+x); }

			protected:
				ImageT& _image;
				Region  _area;

			public:
				/** @brief Pixel-based iterator class.
				 *  @ingroup Gfx
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
				 *  @ingroup Gfx
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
