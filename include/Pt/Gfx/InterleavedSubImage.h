/*
 * Copyright (C) 2006-2007 by Aloysius Indrayanto
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
#ifndef Pt_Gfx_InterleavedSubImage_h
#define Pt_Gfx_InterleavedSubImage_h

#include <Pt/Gfx/Region.h>
#include <Pt/Gfx/ImageAlgo.h>


namespace Pt {

    namespace Gfx {

        //
        // Foward declarations of pixel iterator classes
        //
        template <typename ColorT_> class InterleavedSubImage_PixelIterator;
        template <typename ColorT_> class InterleavedSubImage_ConstPixelIterator;


        /** @brief Subimage class.
         *  @ingroup Gfx
         */
        template <typename ImageT_>
        class /*PT_GFX_API*/ InterleavedSubImage {
            public:
                typedef InterleavedSubImage_PixelIterator<InterleavedSubImage>      PixelIterator;
                typedef InterleavedSubImage_ConstPixelIterator<InterleavedSubImage> ConstPixelIterator;

            public:
                typedef ImageT_  ImageT;

                typedef typename ImageT::ColorT       ColorT;

                typedef typename ImageT::ScanlineT      ScanlineT;
                typedef typename ImageT::ConstScanlineT ConstScanlineT;

                typedef typename ImageT::ColorPtrT      ColorPtrT;
                typedef typename ImageT::ConstColorPtrT ConstColorPtrT;

            public:
                /** @brief Construct a subimage using the given image and area.
                 */
                InterleavedSubImage(ImageT& image, const Pt::Gfx::Region& area);


                /** @brief Comparison based on the pixels' color values.
                 */
                bool operator==(const InterleavedSubImage& src);


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
                InterleavedSubImage& operator=(const ColorT& color);

                /** @brief Fill the subimage with the given image.
                 */
                InterleavedSubImage& operator=(const ImageT& src);

                /** @brief Fill the subimage with the given subimage.
                 */
                InterleavedSubImage& operator=(const InterleavedSubImage& src);


                /** @brief ScanlineT access without range check.
                 */
                inline ScanlineT scanline(int y)
                { return &_image.data()[(y+_area.y())*_image.width() + _area.x()]; }

                /** @brief ScanlineT access without range check.
                 */
                inline ConstScanlineT scanline(int y) const
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

                // Make the pixel iterator classes as friend classes
                friend class InterleavedSubImage_PixelIterator<ImageT_>;
                friend class InterleavedSubImage_ConstPixelIterator<ImageT_>;

            protected:
                ImageT& _image;
                Region  _area;
        };


        /** @internal
            @brief Pixel-based iterator class for InterleavedSubImage<ImageT>.
         *  @ingroup Gfx
         */
        template <typename InterleavedSubImageT_>
        class InterleavedSubImage_PixelIterator {
            public:
                typedef InterleavedSubImageT_                 InterleavedSubImageT;
                typedef typename InterleavedSubImageT::ImageT ImageT;
                typedef typename ImageT::ColorT    ColorT;
                typedef typename ImageT::ColorPtrT ColorPtrT;

            public:
                inline InterleavedSubImage_PixelIterator(InterleavedSubImageT& image)
                : _pixel(&image.scanline(0)[0]),
                  _currentX(0), _width(image.width()),
                  _incr(image.fullImage().width() - image.width() + 1)
                {}

                inline InterleavedSubImage_PixelIterator(InterleavedSubImageT& image, uint x, uint y)
                : _pixel(&image.scanline(y-1)[x-1]),
                  _currentX(0), _width(image.width()),
                  _incr(image.fullImage().width() - image.width() + 1)
                {}

                inline bool operator==(const InterleavedSubImage_PixelIterator& it) const
                { return _pixel == it._pixel; }

                inline bool operator!=(const InterleavedSubImage_PixelIterator& it) const
                { return _pixel != it._pixel; }

                inline ColorT& operator*() const
                { return *_pixel; }

                inline InterleavedSubImage_PixelIterator& operator++()
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
                ColorPtrT _pixel;
                uint      _currentX;
                uint      _width;
                uint      _incr;
        };


        /** @internal
            @brief Pixel-based constant iterator class for InterleavedSubImage<ImageT>.
         *  @ingroup Gfx
         */
        template <typename InterleavedSubImageT_>
        class InterleavedSubImage_ConstPixelIterator {
            public:
                typedef InterleavedSubImageT_                      InterleavedSubImageT;
                typedef typename InterleavedSubImageT::ImageT      ImageT;
                typedef typename ImageT::ColorT         ColorT;
                typedef typename ImageT::ConstColorPtrT ConstColorPtrT;


            public:
                inline InterleavedSubImage_ConstPixelIterator(const InterleavedSubImageT& image)
                : _pixel(&image.scanline(0)[0]),
                  _currentX(0), _width(image.width()),
                  _incr(image.fullImage().width() - image.width() + 1)
                {}

                inline InterleavedSubImage_ConstPixelIterator(const InterleavedSubImageT& image, uint x, uint y)
                : _pixel(&image.scanline(y-1)[x-1]),
                  _currentX(0), _width(image.width()),
                  _incr(image.fullImage().width() - image.width() + 1)
                {}

                inline bool operator==(const InterleavedSubImage_ConstPixelIterator& it) const
                { return _pixel == it._pixel; }

                inline bool operator!=(const InterleavedSubImage_ConstPixelIterator& it) const
                { return _pixel != it._pixel; }

                inline const ColorT& operator*() const
                { return *_pixel; }

                inline InterleavedSubImage_ConstPixelIterator& operator++()
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
                ConstColorPtrT _pixel;
                uint           _currentX;
                uint           _width;
                uint           _incr;
        };

    } // namespace Gfx

} // namespace Pt


//
// NOTE: Why these conditional compilation is always get deleted ???
//
// With GCC we should be able to use explicit template instantiation correctly
// and thus we does not need to include the template implementation header
//
// Unfortunately not under gcc 3.3.4
//#ifndef __GNUC__
#include "InterleavedSubImage.tpp"
//#endif

#endif
