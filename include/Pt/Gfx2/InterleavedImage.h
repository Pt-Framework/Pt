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
#ifndef Pt_Gfx2_InterleavedImage_h
#define Pt_Gfx2_InterleavedImage_h

#include <Pt/Exception.h>
#include <Pt/Gfx2/ARgbFColor.h>
#include <Pt/Math/Rect.h>

#include <vector>


namespace Pt {

	namespace Gfx {

		/** @brief Interleaved image class.
		 *  @ingroup Gfx
		 *
		 *  There are two common memory structures for an image:
		 *    - Interleaved image which is represented by grouping the pixels together
		 *      in memory and interleaving all channels together.
		 *    - Planar images which is represented by keeping the channels in separate
		 *      color planes (blocks of memory). One block to the other blocks may or
		 *      may not be in contiguous memory address.
		 *
		 *  This InterleavedImage<ColorTagT> class is meant to be used for implementing
		 *  interleaved images.
		 */
		template <typename ColorTagT>
		class InterleavedImage {
			public:
				typedef Color<ColorTagT>            ColorT;
				typedef InterleavedImage<ColorTagT> ImageT;

				typedef ColorT*       Scanline;
				typedef const ColorT* ConstScanline;

			public:
				class PixelIterator;
				class ConstPixelIterator;

			public:
				/** @brief The default constructor; will construct an empty image.
				 */
				inline InterleavedImage()
				: _width(0), _height(0)
				{}

				/** @brief Copy constructor.
				 */
				inline InterleavedImage(const ImageT& src)
				: _width(0), _height(0)
				{ *this = src; }

				/** @brief Construct an image with the given size and fill all the pixels with the given color.
				 */
				inline InterleavedImage(uint width_, uint height_, const ColorT& fill = ColorT())
				: _width(0), _height(0)
				{ resize(width_, height_, fill); }


				/** @brief Check if the image is empty or not.
				 */
				inline bool empty() const
				{ return _buff.empty(); }

				/** @brief Return the width of the image.
				 */
				inline uint width() const
				{ return _width; }

				/** @brief Return the height of the image.
				 */
				inline uint height() const
				{ return _height; }


				/** @brief Clears the image (and sets its width and height to 0).
				 */
				inline void clear()
				{ _buff.clear(); _width = 0; _height = 0; }


				/** @brief Resizes the image to a new width and height and let it be initialized using the default color.
				 */
				void resize(uint width_, uint height_);

				/** @brief Resizes the image to a new width and height and fill it with the given color.
				 */
				void resize(uint width_, uint height_, const ColorT& fill);


				/** @brief Assigns a color to all pixels.
				 */
				ImageT& operator=(const ColorT& fill);

				/** @brief Assignment operator from the same image type.
				 */
				ImageT& operator=(const ImageT& src);


				/** @brief Raw data access.
				 */
				inline ColorT* data()
				{ return &_buff[0]; }

				/** @brief Raw data access.
				 */
				inline const ColorT* data() const
				{ return &_buff[0]; }

				/** @brief Scanline access without range check.
				 */
				inline Scanline scanline(int y)
				{ return &_buff[y*_width]; }

				/** @brief Scanline access without range check.
				 */
				inline ConstScanline scanline(int y) const
				{ return &_buff[y*_width]; }

				/** @brief Random pixel access without range check.
				 */
				inline ColorT& pixel(int x, int y)
				{ return _buff[y*_width + x]; }

				/** @brief Random pixel access without range check.
				 */
				inline const ColorT& pixel(int x, int y) const
				{ return _buff[y*_width + x]; }

				/** @brief Random pixel access with range check.
				 */
				ColorT& at(int x, int y);

				/** @brief Random pixel access with range check.
				 */
				const ColorT& at(int x, int y) const;

				/** @brief Return the color at the specified coordinates.
				  *
				  * If the coordinates are out of range, the given 'invalid' color
				  * will be returned instead.
				 */
				const ColorT& color(int x, int y, const ColorT& invalid = ColorT() ) const;

				/** @brief Set the color at the specified coordinates.
				 */
				void setColor(int x, int y, const ColorT& color_);


				/** @brief Return an iterator indicating the position of the first pixel in the image.
				 */
				inline PixelIterator begin()
				{ return PixelIterator(*this); }

				/** @brief Return an iterator indicating the position after the last pixel in the image.
				 */
				inline PixelIterator end()
				{ return PixelIterator(*this, this->width(), this->height()-1); }

				/** @brief Return an iterator indicating the position of a pixel at(x,y).
				 */
				inline PixelIterator iterator(uint y, uint x)
				{ return PixelIterator(*this, y, x); }

				/** @brief Return a constant iterator indicating the position of the first pixel in the image.
				 */
				inline ConstPixelIterator begin() const
				{ return ConstPixelIterator(*this); }

				/** @brief Return a constant iterator indicating the position after the last pixel in the image.
				 */
				inline ConstPixelIterator end() const
				{ return ConstPixelIterator(*this, this->width(), this->height()-1); }

				/** @brief Return a constant iterator indicating the position of a pixel at(x,y).
				 */
				inline ConstPixelIterator iterator(uint y, uint x) const
				{ return ConstPixelIterator( *this, y, x ); }


			protected:
				std::vector<ColorT> _buff;
				size_t              _width;
				size_t              _height;

			public:
				/** @brief Pixel-based iterator class.
				 *  @ingroup Gfx
				 */
				class PixelIterator
				{
					public:
						typedef InterleavedImage<ColorTagT> ImageT;
						typedef typename ImageT::ColorT     ColorT;

					public:
						inline PixelIterator(ImageT& image, uint x = 0, uint y = 0)
						: _image(&image), _pixel(&image.scanline(y)[x])
						{}

						inline PixelIterator operator=(PixelIterator other)
						{
							_pixel = other._pixel;
							_image = other._image;
							return *this;
						}

						inline bool operator!=(const PixelIterator& it) const
						{ return this->_pixel != it._pixel; }

						inline ColorT& operator*() const
						{ return *_pixel; }

						inline PixelIterator& operator++()
						{ ++_pixel; return *this; }

						inline PixelIterator operator+=(size_t n)
						{ _pixel += n; return *this; }

						inline Math::Size operator-(const PixelIterator& other)
						{
							const size_t pos         = _pixel - _image->data();
							const size_t otherPos    = other._pixel - other._image->data();
							const size_t otherWidth  = otherPos / other._image->height();
							const size_t otherHeight = otherPos / other._image->width();

							const size_t width  = pos / _image->height();
							const size_t height = pos / _image->width();

							return Math::Size(width - otherWidth, height -otherHeight);
						}

					private:
						ImageT* _image;
						ColorT* _pixel;
				};

				/** @brief Pixel-based constant iterator class.
				 *  @ingroup Gfx
				 */
				class ConstPixelIterator
				{
					public:
						typedef InterleavedImage<ColorTagT> ImageT;
						typedef typename ImageT::ColorT     ColorT;

					public:
						inline ConstPixelIterator(const ImageT& image, uint x = 0, uint y = 0)
						: _image(&image), _pixel(&image.scanline(y)[x])
						{}

						inline ConstPixelIterator operator=(ConstPixelIterator other)
						{
							_pixel = other._pixel;
							_image = other._image;
							return *this;
						}

						inline bool operator!=(const ConstPixelIterator& it) const
						{ return this->_pixel != it._pixel; }

						inline const ColorT& operator*() const
						{ return *_pixel; }

						inline ConstPixelIterator& operator++()
						{ ++_pixel; return *this; }

						inline ConstPixelIterator operator+=(size_t n)
						{ _pixel += n; return *this; }

						inline Math::Size operator-(const ConstPixelIterator& other)
						{
							const size_t pos         = _pixel - _image->data();
							const size_t otherPos    = other._pixel - other._image->data();
							const size_t otherWidth  = otherPos / other._image->height();
							const size_t otherHeight = otherPos / other._image->width();

							const size_t width  = pos / _image->height();
							const size_t height = pos / _image->width();

							return Math::Size(width - otherWidth, height -otherHeight);
						}

					private:
						const ImageT* _image;
						const ColorT* _pixel;
				};
		};

	} // namespace Gfx

} // namespace Pt

#endif

