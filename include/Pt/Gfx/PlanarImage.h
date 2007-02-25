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
#ifndef Pt_Gfx_PlanarImage_h
#define Pt_Gfx_PlanarImage_h

#include <Pt/Exception.h>
#include <Pt/Gfx/ARgbFColorRef.h>

#include <vector>


namespace Pt {

    namespace Gfx {

        /** @brief Planar image class.
         *  @ingroup Gfx
         *
         *  There are two common memory structures for an image:
         *    - Interleaved image which is represented by grouping the pixels together
         *      in memory and interleaving all channels together.
         *    - Planar images which is represented by keeping the channels in separate
         *      color planes (blocks of memory). One block to the other blocks may or
         *      may not be in contiguous memory address.
         *
         *  This PlanarImage<typename ColorRefT, typename ColorTraitsT> class is
         *  meant to be used for implementing planar images.
         */
        template <typename ColorRefT_, typename ColorTraitsT_ = ColorTraits< Color<ColorRefT_> > >
        class /*PT_GFX_API*/ PlanarImage {
            public:
                typedef typename ColorRefT_::NonRefT ColorT;
                typedef ColorTraitsT_                ColorTraitsT;

                typedef typename ColorTraitsT::ScanlineT      ScanlineT;
                typedef typename ColorTraitsT::ConstScanlineT ConstScanlineT;

                typedef typename ColorTraitsT::ColorPtrT      ColorPtrT;
                typedef typename ColorTraitsT::ConstColorPtrT ConstColorPtrT;

                //
                // Below are specific to planar image only
                //
                typedef ColorRefT_                        ColorRefT;
                typedef typename ColorTraitsT::ComponentT ComponentT;

            public:
                class PixelIterator;
                class ConstPixelIterator;

            public:
                /** @brief The default constructor; will construct an empty image.
                 */
                inline PlanarImage()
                : _width(0), _height(0)
                {}

                /** @brief Copy constructor.
                 */
                inline PlanarImage(const PlanarImage& src)
                : _width(0), _height(0)
                { *this = src; }

                /** @brief Construct an image with the given size and fill all the pixels with the given color.
                 */
                inline PlanarImage(uint width_, uint height_, const ColorT& fill = ColorT() )
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
                { _buff.clear(); _chanPtr.clear(); _chanSize.clear(); _width = 0; _height = 0; }


	              /** @brief Resizes the image to a new width and height and let it be initialized using the default color.
                 */
                void resize(uint width_, uint height_);

                /** @brief Resizes the image to a new width and height and fill it with the given color.
                 */
                void resize(uint width_, uint height_, const ColorT& fill);


                /** @brief Assigns a color to all pixels.
                 */
                PlanarImage& operator=(const ColorT& fill);

                /** @brief Assignment operator from the same image type.
                 */
                PlanarImage& operator=(const PlanarImage& src);


                /** @brief Raw data access.
                 */
                inline ComponentT* data()
                { return &_buff[0]; }

                /** @brief Raw data access.
                 */
                inline const ComponentT* data() const
                { return &_buff[0]; }


                /** @brief Scanline access without range check.
                 */
                inline ScanlineT scanline(int y)
                { return ScanlineT(_chanPtr, _width, y); }

                /** @brief Scanline access without range check.
                 */
                inline ConstScanlineT scanline(int y) const
                { return ConstScanlineT(_chanPtr, _width, y); }


                /** @brief Random pixel access without range check.
                 */
                inline ColorRefT pixel(int x, int y)
                { return *ColorPtrT(_chanPtr, _width, _height, x, y); }

                /** @brief Random pixel access without range check.
                 */
                inline const ColorT pixel(int x, int y) const
                {
                    ColorT col;
                    assign(col, *ConstColorPtrT(_chanPtr, _width, _height, x, y));
                    return col;
                }

                /** @brief Random pixel access with range check.
                 */
                ColorRefT at(int x, int y);

                /** @brief Random pixel access with range check.
                 */
                const ColorT at(int x, int y) const;

                /** @brief Return the color at the specified coordinates.
                  *
                  * If the coordinates are out of range, the given 'invalid' color
                  * will be returned instead.
                 */
                const ColorT color(int x, int y, const ColorT& invalid = ColorT() ) const;

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
                struct ChanSize {
                    size_t width;
                    size_t height;
                };

                std::vector<ComponentT>  _buff;
                std::vector<ComponentT*> _chanPtr;
                std::vector<size_t>      _chanSize;
                size_t                   _width;
                size_t                   _height;

            public:
                /** @brief Pixel-based iterator class.
                 *  @ingroup Gfx
                 */
                class PixelIterator
                {
                    public:
                        typedef PlanarImage<ColorRefT_, ColorTraitsT_> ImageT;
                        typedef typename ImageT::ColorRefT             ColorRefT;
                        typedef typename ImageT::ColorTraitsT          ColorTraitsT;

                    public:
                        inline PixelIterator()
                        : _image(0), _pixel(0)
                        {}

                        inline PixelIterator(ImageT& image, uint x = 0, uint y = 0)
                        : _image(&image), _pixel(ColorPtrT(image._chanPtr, image._width, image._height, x, y))
                        {}

                        inline PixelIterator operator=(PixelIterator other)
                        {
                            _pixel = other._pixel;
                            _image = other._image;
                            return *this;
                        }

                        inline bool operator!=(const PixelIterator& it) const
                        { return this->_pixel != it._pixel; }

                        inline ColorRefT operator*()
                        { return *_pixel; }

                        inline PixelIterator& operator++()
                        { ++_pixel; return *this; }

                        inline PixelIterator operator+=(size_t n)
                        { _pixel += n; return *this; }

                        inline Math::Size operator-(const PixelIterator& other) const
                        {

                            const Math::Point& a = _pixel.currentXYPosition();
                            const Math::Point& b = other._pixel.currentXYPosition();

                            return Math::Size(a.x()-b.x(), a.y()-b.y());
                        }

                    private:
                        ImageT*   _image;
                        ColorPtrT _pixel;
                };

                /** @brief Pixel-based constant iterator class.
                 *  @ingroup Gfx
                 */
                class ConstPixelIterator
                {
                    public:
                        typedef PlanarImage<ColorRefT_, ColorTraitsT_> ImageT;
                        typedef typename ImageT::ColorRefT             ColorRefT;
                        typedef typename ImageT::ColorTraitsT          ColorTraitsT;

                    public:
                        inline ConstPixelIterator()
                        : _image(0), _pixel(0)
                        {}

                        inline ConstPixelIterator(const ImageT& image, uint x = 0, uint y = 0)
                        : _image(&image), _pixel(ConstColorPtrT(image._chanPtr, image._width, image._height, x, y))
                        {}

                        inline ConstPixelIterator operator=(ConstPixelIterator other)
                        {
                            _pixel = other._pixel;
                            _image = other._image;
                            return *this;
                        }

                        inline bool operator!=(const ConstPixelIterator& it) const
                        { return this->_pixel != it._pixel; }

                        inline const ColorT operator*()
                        {
                            ColorT col;
                            assign(col, *_pixel);
                            return col;
                        }

                        inline ConstPixelIterator& operator++()
                        { ++_pixel; return *this; }

                        inline ConstPixelIterator operator+=(size_t n)
                        { _pixel += n; return *this; }

                        inline Math::Size operator-(const ConstPixelIterator& other) const
                        {

                            const Math::Point& a = _pixel.currentXYPosition();
                            const Math::Point& b = other._pixel.currentXYPosition();

                            return Math::Size(a.x()-b.x(), a.y()-b.y());
                        }

                    private:
                        const ImageT*  _image;
                        ConstColorPtrT _pixel;
                };
        };

    } // namespace Gfx

} // namespace Pt


//
// NOTE: Why these conditional compilation is always get deleted ???
//
// With GCC we should be able to use explicit template instantiation correctly
// and thus we does not need to include the template implementation header
#ifndef __GNUC__
#include "PlanarImage.tpp"
#endif

#endif

