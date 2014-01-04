/*
 * Copyright (C) 2006-2010 by Aloysius Indrayanto
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Gfx_InterleavedImage_h
#define Pt_Gfx_InterleavedImage_h

#include <Pt/Gfx/ARgbFColor.h>
#include <Pt/Gfx/Rect.h>
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <vector>


namespace Pt {

    namespace Gfx {


        template <typename ColorT_> class InterleavedImage_PixelIterator;
        template <typename ColorT_> class InterleavedImage_ConstPixelIterator;


        /** @brief Interleaved image class.
         *
         *  There are two common memory structures for an image:
         *    - Interleaved image which is represented by grouping the pixels together
         *      in memory and interleaving all channels together.
         *    - Planar images which is represented by keeping the channels in separate
         *      color planes (blocks of memory). One block to the other blocks may or
         *      may not be in contiguous memory address.
         *
         *  This InterleavedImage<typename ColorT> class is
         *  meant to be used for implementing interleaved images.
         */
        template <typename ColorT_>
        class InterleavedImage {
            public:
                typedef InterleavedImage_PixelIterator<ColorT_>      PixelIterator;
                typedef InterleavedImage_ConstPixelIterator<ColorT_> ConstPixelIterator;

            public:
                typedef ColorT_       ColorT;

                typedef ColorT*       ColorPtrT;
                typedef const ColorT* ConstColorPtrT;

                typedef ColorT*       ScanlineT;
                typedef const ColorT* ConstScanlineT;

            public:
                /** @brief The default constructor; will construct an empty image.
                 */
                inline InterleavedImage()
                : _width(0), _height(0)
                {}

                /** @brief Copy constructor.
                 */
                inline InterleavedImage(const InterleavedImage& src)
                : _width(0), _height(0)
                { *this = src; }

                /** @brief Construct an image with the given size and fill all the pixels with the given color.
                 */
                inline InterleavedImage(uint width_, uint height_, const ColorT& fill = ColorT() )
                : _width(0), _height(0)
                { resize(width_, height_, fill); }


                /** @brief Check if the image is empty or not.
                 */
                inline bool empty() const
                { return _buff.empty(); }

                /** @brief Return the width of the image.
                 */
                inline size_t width() const
                { return _width; }

                /** @brief Return the height of the image.
                 */
                inline size_t height() const
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
                InterleavedImage& operator=(const ColorT& fill);

                /** @brief Assignment operator from the same image type.
                 */
                InterleavedImage& operator=(const InterleavedImage& src);

                /** @brief Raw data access.
                 */
                inline ColorPtrT data()
                { return &_buff[0]; }

                /** @brief Raw data access.
                 */
                inline ConstColorPtrT data() const
                { return &_buff[0]; }


                /** @brief ScanlineT access without range check.
                 */
                inline ScanlineT scanline(int y)
                { return &_buff[y*_width]; }

                /** @brief ScanlineT access without range check.
                 */
                inline ConstScanlineT scanline(int y) const
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

                // Make the pixel iterator classes as friend classes
                friend class InterleavedImage_PixelIterator<ColorT_>;
                friend class InterleavedImage_ConstPixelIterator<ColorT_>;

            protected:
                std::vector<ColorT> _buff;
                size_t              _width;
                size_t              _height;
        };


        /** @internal
            @brief Pixel-based iterator class for InterleavedImage<ColorT>.
         *  @ingroup Gfx
         */
        template <typename ColorT_>
        class InterleavedImage_PixelIterator
        {
            public:
                typedef InterleavedImage<ColorT_>  ImageT;
                typedef typename ImageT::ColorT    ColorT;
                typedef typename ImageT::ColorPtrT ColorPtrT;

            public:
                inline InterleavedImage_PixelIterator()
                : _image(0), _pixel(0)
                {}

                inline InterleavedImage_PixelIterator(ImageT& image, uint x = 0, uint y = 0)
                : _image(&image), _pixel(&image.scanline(y)[x])
                {}

                inline InterleavedImage_PixelIterator operator=(InterleavedImage_PixelIterator other)
                {
                    _pixel = other._pixel;
                    _image = other._image;
                    return *this;
                }

                inline bool operator!=(const InterleavedImage_PixelIterator& it) const
                { return this->_pixel != it._pixel; }

                inline ColorT& operator*()
                { return *_pixel; }

                inline InterleavedImage_PixelIterator& operator++()
                { ++_pixel; return *this; }

                inline InterleavedImage_PixelIterator operator+=(size_t n)
                { _pixel += n; return *this; }

                inline Gfx::Size operator-(const InterleavedImage_PixelIterator& other) const
                {
                    const size_t pos    = _pixel - _image->data();
                    const size_t width  = pos / _image->height();
                    const size_t height = pos / _image->width();

                    const size_t otherPos    = other._pixel - other._image->data();
                    const size_t otherWidth  = otherPos / other._image->height();
                    const size_t otherHeight = otherPos / other._image->width();

                    return Gfx::Size(width - otherWidth, height - otherHeight);
                }

            private:
                ImageT*   _image;
                ColorPtrT _pixel;
        };


        /** @internal
            @brief Pixel-based constant iterator class for InterleavedImage<ColorT>.
         *  @ingroup Gfx
         */
        template <typename ColorT_>
        class InterleavedImage_ConstPixelIterator
        {
            public:
                typedef InterleavedImage<ColorT_>       ImageT;
                typedef typename ImageT::ColorT         ColorT;
                typedef typename ImageT::ConstColorPtrT ConstColorPtrT;

            public:
                inline InterleavedImage_ConstPixelIterator()
                : _image(0) , _pixel(0)
                {}

                inline InterleavedImage_ConstPixelIterator(const ImageT& image, uint x = 0, uint y = 0)
                : _image(&image), _pixel(&image.scanline(y)[x])
                {}

                inline InterleavedImage_ConstPixelIterator operator=(InterleavedImage_ConstPixelIterator other)
                {
                    _pixel = other._pixel;
                    _image = other._image;
                    return *this;
                }

                inline bool operator!=(const InterleavedImage_ConstPixelIterator& it) const
                { return this->_pixel != it._pixel; }

                inline const ColorT& operator*() const
                { return *_pixel; }

                inline InterleavedImage_ConstPixelIterator& operator++()
                { ++_pixel; return *this; }

                inline InterleavedImage_ConstPixelIterator operator+=(size_t n)
                { _pixel += n; return *this; }

                inline Gfx::Size operator-(const InterleavedImage_ConstPixelIterator& other) const
                {
                    const size_t pos    = _pixel - _image->data();
                    const size_t width  = pos / _image->height();
                    const size_t height = pos / _image->width();

                    const size_t otherPos    = other._pixel - other._image->data();
                    const size_t otherWidth  = otherPos / other._image->height();
                    const size_t otherHeight = otherPos / other._image->width();

                    return Gfx::Size(width - otherWidth, height - otherHeight);
                }

            private:
                const ImageT*  _image;
                ConstColorPtrT _pixel;
        };


    } // namespace Gfx

} // namespace Pt


//#ifndef __GNUC__
#include "InterleavedImage.tpp"
//#endif

#endif

