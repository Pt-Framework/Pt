/***************************************************************************
 *   Copyright (C) 2007 by Aloysius Indrayanto                             *
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx_PlanarImageModel1x1_h
#define Pt_Gfx_PlanarImageModel1x1_h

#include <Pt/Gfx/PlanarImageModel.h>


namespace Pt {

    namespace Gfx {

        /** @brief Planar image model class for non-subsampled planar images.
         *  @internal
         */
        template<typename ColorModelT>
        class PlanarImageView<ColorModelT, 1, 1> {
            public:
                typedef typename ColorModelT::Color Color;

                typedef typename ColorModelT::ConstColor ConstColor;

                typedef typename ColorModelT::ColorRef ColorRef;

                typedef typename ColorModelT::ConstColorRef ConstColorRef;

                typedef typename ColorModelT::Component Component;

                typedef typename ColorModelT::Color ValueT;

                typedef typename ColorModelT::ColorData ColorData;

                typedef typename ColorModelT::ConstColorData ConstColorData;

                class ColorPtrT;

                class ConstColorPtrT;

                // Pixel-iterator class for the color model
                class PixelIterator;

                // Pixel-iterator class for the color model
                class ConstPixelIterator;

                // Allocate the needed memory for the image
                //inline void alloc(size_t imageWidth, size_t imageHeight);

                //! @brief Returns the required memory for the image data
                inline Pt::size_t size(size_t width, size_t height);

                //! @brief Initialises the model to a given memory block
                inline void init(unsigned char* memory, size_t width, size_t height);

            public:
                inline PlanarImageView()
                : _width(0), _height(0)
                {}

                // Make the image class as a friend class
                friend class PlanarImage<PlanarImageView>;

                size_t width() const
                { return _width; }

                size_t height() const
                { return _height; }

            private:
                // Data
                // std::vector<ComponentT> _buff;    // Image's data
                ColorData _chanPtr; // List of pointers to each channel starting area
                size_t    _width;   // Image's width
                size_t    _height;  // Image's height
        };

    } // namespace Gfx

} // namespace Pt


//
// Include the template implementation header
//
#include "PlanarImageModel1x1.tpp"

#endif

