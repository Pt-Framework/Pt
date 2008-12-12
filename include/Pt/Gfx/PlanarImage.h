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
#ifndef Pt_Gfx_PlanarImage_h
#define Pt_Gfx_PlanarImage_h

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/PlanarImageView.h>
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <vector>


namespace Pt {

    namespace Gfx {

        /** @brief Planar image class.
         */
        template < typename ImageViewT, typename AllocatorT >
        class PlanarImage {
            public:
                typedef ImageViewT View;

                typedef AllocatorT Allocator;

                typedef typename View::Color Color;

                typedef typename View::PixelIterator PixelIterator;

                typedef typename View::ConstPixelIterator ConstPixelIterator;

            public:
                /** @brief The default constructor; will construct an empty image.
                 */
                PlanarImage( const Allocator& a = Allocator() )
                : _memory(0)
                , _size(0)
                , _alloc(a)
                {}
#if 0
                /** @brief Copy constructor.
                 */
                inline PlanarImage(const PlanarImage& src)
                : _width(0), _height(0)
                { *this = src; }
#endif
                /** @brief Construct an image with the given size and fill all the pixels with the given color.
                 */
                PlanarImage(uint width, uint height, const Color& fill = Color(), const Allocator& a = Allocator() )
                : _memory(0)
                , _size(0)
                , _alloc(a)
                {
                    Pt::size_t bytes = _view.size(width, height);
                    _memory = _alloc.allocate(bytes);
                    _view.init(_memory, width, height); // should not throw
                    _size = bytes;
                }

                ~PlanarImage( )
                {
                    _alloc.deallocate(_memory, _size);
                }

                View view() const
                { return _view; }

                /** @brief Check if the image is empty or not.
                 */
                inline bool empty() const
                { return _size == 0; }

                /** @brief Return the width of the image.
                 */
                inline uint width() const
                { return _view.width(); }

                /** @brief Return the height of the image.
                 */
                inline uint height() const
                { return _view.height(); }

#if 0

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
                inline ColorProxyT pixel(int x, int y)
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
                ColorProxyT at(int x, int y);

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
#endif
            protected:
                ImageViewT _view;

            private:
                unsigned char* _memory;
                size_t _size;
                Allocator _alloc;
        };

    } // namespace Gfx

} // namespace Pt


#ifndef __GNUC__
#include "PlanarImage.tpp"
#endif

#endif
