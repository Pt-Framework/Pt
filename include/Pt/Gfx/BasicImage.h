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
#ifndef Pt_BasicImage_h
#define Pt_BasicImage_h

#include <Pt/Exception.h>
#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicColor.h>
#include <Pt/Math/Rect.h>

#include <vector>


namespace Pt {

	namespace Gfx {

		//!
		//! \brief BasicImage<ColorSpaceT> class
		//!
		//! There are two common memory structures for an image:
		//!   - Interleaved image which is represented by grouping the pixels together
		//!     in memory and interleaving all channels together.
		//!   - Planar images which is represented by keeping the channels in separate
		//!     color planes (blocks of memory). One block to the other blocks may or
		//!     may not be in contiguous memory address.
		//!
		//! The BasicImage<ColorSpaceT> class is meant to be used for implementing
		//! interleaved images altough it can be used to implement planar images too
		//! (by using helper classes for accessing pixels).
		//! \n \n
		//! If one wants to fully specialize this image class, one must at least
		//! implement the functions defined here, however adding as many as additional
		//! functions is allowed.
		//! \n \n
		//! Note that incomplete implementation of a fully specialized image classes
		//! are allowed for special use case and should be docummented clearly.
		//! \n \n
		//! For each new color model to be used as new image type, one must include
		//! the implementation header file of BasicImage<T>, "BasicImage.tpp" and
		//! perform explicit instantiation. Example:
		//!
		//!   struct MyColorSpace {};
		//!
		//!   template <>
		//!   class PT_API BasicColor<MyColorSpace> {
		//!     ...
		//!     ...
		//!     ...
		//!   };
		//!
		//!   template BasicImage<MyColorSpace>;
		//!
		//!
		//!   typedef BasicColor<MyColorSpace> MyColorType;
		//!   typedef BasicImage<MyColorSpace> MyImageType;
		//!
		template <typename C>
		class BasicImage {
			public:
				typedef C                       ColorSpaceT;
				typedef BasicColor<ColorSpaceT> ColorT;
				typedef BasicImage<ColorSpaceT> ImageT;

				typedef ColorT*       Scanline;
				typedef const ColorT* ConstScanline;

			protected:
				class PixelIterator;
				class ConstPixelIterator;

			public:
				//! Default ctor, will construct an empty image
				inline BasicImage()
				: _width(0), _height(0)
				{}

				//! Copy ctor for the same image type
				inline BasicImage(const BasicImage<ColorSpaceT>& src)
				: _width(0), _height(0)
				{ *this = src; }

				//! @brief Contructs from an image using a different color space
				template <typename CS> inline
				BasicImage(const BasicImage<CS>& other)
				: _width(0), _height(0)
				{
					this->resize( other.width(), other.height() );
					this->assign( other.begin(), other.end(), this-begin() );
				}

				//! Construct an image with the given size and
				//! fill all the pixels with the given color
				inline BasicImage(uint width_, uint height_, const ColorT& fill = ColorT())
				: _width(0), _height(0)
				{ resize(width_, height_, fill); }

				//! Clears the image (and sets its width and height to 0)
				inline void clear()
				{ _buff.clear(); _width = 0; _height = 0; }

				//! Resizes (and clear) the image to a new width and height
				void resize(uint width_, uint height_, const ColorT& fill = ColorT());

				//! Assigns a color from the same color type to all pixels
				BasicImage<ColorSpaceT>& operator=(const ColorT& fill);

				//! Assigns a color from different color type to all pixels
				template <typename SrcColorSpaceT>
				inline BasicImage<ColorSpaceT>& operator=(const BasicColor<SrcColorSpaceT>& fill)
				{ ColorT tmp(fill); return this->operator=(tmp); }

				//! Assignment operator from the same image type
				BasicImage<ColorSpaceT>& operator=(const BasicImage<ColorSpaceT>& src);

				//! @brief Assign image of different color space
				template <typename CS>
				inline BasicImage<ColorSpaceT>& operator=(const BasicImage<CS>& other)
				{
					this->resize( other.width(), other.height() );
					assign( other.begin(), other.end(), this->begin() );
					return *this;
				}

				//! Check if the image is empty or not
				inline bool empty() const
				{ return _buff.empty(); }

				//! Returns the width of the image
				inline uint width() const
				{ return _width; }

				//! Returns the height of the image
				inline uint height() const
				{ return _height; }

				//! Raw data access
				inline ColorT* data()
				{ return &_buff[0]; }

				//! Raw data access
				inline const ColorT* data() const
				{ return &_buff[0]; }

				//! Scanline access without range check
				inline Scanline scanline(int y)
				{ return &_buff[y*_width]; }

				//! Scanline access without range check
				inline ConstScanline scanline(int y) const
				{ return &_buff[y*_width]; }

				//! Random access without range check
				inline ColorT& pixel(int x, int y)
				{ return _buff[y*_width + x]; }

				//! Random access without range check
				inline const ColorT& pixel(int x, int y) const
				{ return _buff[y*_width + x]; }

				//! Random access with range check
				ColorT& at(int x, int y);

				//! Random access with range check
				const ColorT& at(int x, int y) const;

				//! Returns the color at the specified coordinates, if the
				//! coordinates are out of range, the given "invalid" color
				//! will be returned
				//! TODO: find better name
				const ColorT& color(int x, int y, const ColorT& invalid = ColorT() ) const;

				//! Set the color at the specified coordinates
				void setColor(int x, int y, const ColorT& color_);

				//! Returns an iterator indicating the position
				//! of the first pixel in this image
				inline PixelIterator begin()
				{ return PixelIterator(*this); }

				//! Returns an iterator indicating the position
				//! after the last pixel in this image
				inline PixelIterator end()
				{ return PixelIterator(*this, this->width(), this->height()-1); }

				//! Returns an iterator indicating the position
				//! of a pixel at(x,y)
				inline PixelIterator iterator(uint y, uint x)
				{ return PixelIterator(*this, y, x); }

				//! Returns a const iterator indicating the position
				//! of the first pixel in this image
				inline ConstPixelIterator begin() const
				{ return ConstPixelIterator(*this); }

				//! Returns a const iterator indicating the position
				//! after the last pixel in this image
				inline ConstPixelIterator end() const
				{ return ConstPixelIterator(*this, this->width(), this->height()-1); }

				//! Returns a const iterator indicating the position
				//! of a pixel at(x,y)
				inline ConstPixelIterator iterator(uint y, uint x) const
				{ return ConstPixelIterator( *this, y, x ); }


			protected:
				std::vector<ColorT> _buff;
				size_t _width;
				size_t _height;

				/** \brief Pixel-based iterator
				*/
				class PixelIterator
				{
					public:
						typedef BasicImage<ColorSpaceT> ImageT;
						typedef typename ImageT::ColorT ColorT;

					public:
						PixelIterator(ImageT& image, uint x = 0, uint y = 0)
						: _image(&image), _pixel(&image.scanline(y)[x]) {}

						PixelIterator operator=(PixelIterator other)
						{
							_pixel = other._pixel;
							_image = other._image;
							return *this;
						}

						ColorT& operator*() const
						{ return *_pixel; }

						PixelIterator operator+=(size_t n)
						{ _pixel += n; return *this; }

						PixelIterator& operator++()
						{ ++_pixel; return *this; }

						bool operator!=(const PixelIterator& it) const
						{ return this->_pixel != it._pixel; }

						Math::Size operator-(const PixelIterator& other)
						{
							const size_t pos = _pixel - _image->data();
							const size_t otherPos = other._pixel - other._image->data();

							const size_t otherWidth = otherPos / other._image->height();
							const size_t otherHeight = otherPos / other._image->width();

							const size_t width = pos / _image->height();
							const size_t height = pos / _image->width();

							return Math::Size(width - otherWidth, height -otherHeight);
						}

					private:
						ImageT* _image;
						ColorT* _pixel;
				};


				/** \brief Const pixel-based iterator
				*/
				class ConstPixelIterator
				{
					public:
						typedef BasicImage<ColorSpaceT> ImageT;
						typedef typename ImageT::ColorT ColorT;

					public:
						ConstPixelIterator(const ImageT& image, uint x = 0, uint y = 0)
						: _image(&image), _pixel(&image.scanline(y)[x])
						{}

						ConstPixelIterator operator=(ConstPixelIterator other)
						{
							_pixel = other._pixel;
							_image = other._image;
							return *this;
						}

						const ColorT& operator*() const
						{ return *_pixel; }

						ConstPixelIterator& operator++()
						{ ++_pixel; return *this; }

						ConstPixelIterator operator+=(size_t n)
						{ _pixel += n; return *this; }

						bool operator!=(const ConstPixelIterator& it) const
						{ return this->_pixel != it._pixel; }

						Math::Size operator-(const ConstPixelIterator& other)
						{
							const size_t pos = _pixel - _image->data();
							const size_t otherPos = other._pixel - other._image->data();

							const size_t otherWidth = otherPos / other._image->height();
							const size_t otherHeight = otherPos / other._image->width();

							const size_t width = pos / _image->height();
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

#include <Pt/Gfx/BasicImage.tpp>

#endif

