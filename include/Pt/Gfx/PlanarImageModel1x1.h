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

        /** @brief Implementation of a pointer to a color in planar images
            @internal

            This class derives as many times recursively from itself, as
            the color model has planes. Each derivation handles a pointer
            to an element in a plane. ComponentT is the value type of
            an element in a plane. NumPlanes is the total number of planes.
            N indexes the plane this class handles.
        */
        template <typename ComponentT, size_t NumPlanes, size_t N = NumPlanes-1>
        struct ColorPtrImpl : public ColorPtrImpl<ComponentT, NumPlanes, N-1>
        {
            ColorPtrImpl()
            { }

            ColorPtrImpl(ComponentT** c)
            : ColorPtrImpl<ComponentT, NumPlanes, N-1>(c)
            { this->_components[N] = c[N]; }

            /** @brief Set component pointer to appropriate offsets
            */
            void set(ComponentT* data, size_t xpos, size_t ypos, size_t width)
            {
                const size_t planeOffset = (N * width);
                const size_t elemOffset = (xpos + (ypos * width));
                this->_components[N] = data + planeOffset + elemOffset;

                ColorPtrImpl<ComponentT, NumPlanes, N-1>::set(data, xpos, ypos, width);
            }

            void inc()
            {
                ++this->_components[N];
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::inc();
            }

            void dec()
            {
                --this->_components[N];
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::inc();
            }

            void advance(size_t val)
            {
                this->_components[N] += val;
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::advance(val);
            }

            void rewind(size_t val)
            {
                this->_components[N] -= val;
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::rewind(val);
            }

            bool equals(const ColorPtrImpl& other) const
            {
                if(this->_components[N] != other._components[N]) return false;
                return ColorPtrImpl<ComponentT, NumPlanes, N-1>::equals(other);
            }

            bool notEquals(const ColorPtrImpl& other) const
            {
                if(this->_components[N] != other._components[N]) return true;
                return ColorPtrImpl<ComponentT, NumPlanes, N-1>::notEquals(other);
            }
        };


        /** @brief Terminates recursive derivation
            @internal

            This class has the actual array of pointers that point to
            the component values in the planes of a planar image. It
            also handles the 0th plane.
        */
        template <typename ComponentT, size_t NumPlanes>
        struct ColorPtrImpl<ComponentT, NumPlanes, 0>
        {
            ColorPtrImpl()
            { }

            ColorPtrImpl(ComponentT** c)
            { _components[0] = c[0]; }

            ComponentT** components()
            { return _components; }

            ComponentT* const* components() const
            { return _components; }

            void set(ComponentT* data, size_t xpos, size_t ypos, size_t width)
            {
                const size_t offset = (ypos * width) + xpos;
                this->_components[0] = data + offset;
            }

            void inc()
            {
                ++_components[0];
            }

            void dec()
            {
                --_components[0];
            }

            void advance(size_t val)
            {
                _components[0] += val;
            }

            void rewind(size_t val)
            {
                _components[0] -= val;
            }

            bool equals(const ColorPtrImpl& other) const
            {
                 return _components[0] == other._components[0];
            }

            bool notEquals(const ColorPtrImpl& other) const
            {
                return _components[0] != other._components[0];
            }

            ComponentT* _components[NumPlanes];
        };


        /** @brief Planar image model class for non-subsampled planar images.
         *  @internal
         *
         *  For now it just an empty foward declaration but later it will become
         *  the generic implementation of the planar image model.
         */
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1> {
            public:
                // Color proxy type type (value type) of the color model
                typedef ColorProxyT_ ColorProxyT;

                // Number of channels of the color model
                static const size_t NumberOfChannels = ColorProxyT_::NumberOfChannels;

                // Non-reference type (value type) of the color
                typedef typename ColorProxyT::ValueT ValueT;

                // Value type of each individual component of the color model
                typedef typename ColorProxyT::ComponentT ComponentT;

                // Vector of channels' pointers
                typedef ComponentT* PixelData[NumberOfChannels];

                // Vector of channels' constant pointers
                typedef const ComponentT* ConstPixelData[NumberOfChannels];

                // Color pointer class for the color model
                class ColorPtrT;

                // Constant color pointer class for the color model
                class ConstColorPtrT;

                // Scanline class for the color model
                class ScanlineT;

                // Constant scanline class for the color model
                class ConstScanlineT;
        };

    } // namespace Gfx

} // namespace Pt


//
// Include the template implementation header
//
#include "PlanarImageModel1x1.tpp"

#endif

