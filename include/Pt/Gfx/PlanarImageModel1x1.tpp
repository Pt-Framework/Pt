/***************************************************************************
 *   Copyright (C) 2007 by Aloysius Indrayanto                             *
 *   Copyright (C) 2007 by Marc Boris Duerner                               *
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
#ifndef Pt_Gfx_PlanarImageModel1x1_tpp
#define Pt_Gfx_PlanarImageModel1x1_tpp


namespace Pt {

    namespace Gfx {


        template<typename ColorProxyT_>
        const size_t PlanarImageModel<ColorProxyT_, 1, 1>::NumberOfChannels;


        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ColorPtrT
        {
            friend class PlanarImage<PlanarImageModel>;

            public:
                inline ColorPtrT(void*)
                : _imgW(0)
                , _imgH(0)
                , _beginPlanes(0)
                {}

                inline ColorPtrT(ComponentT* data,
                                 size_t imageWidth, size_t imageHeight,
                                 size_t posX, size_t posY)
                : _imgW(imageWidth)
                , _imgH(imageHeight)
                , _beginPlanes(data)
                {
                    _impl.set(data, posX, posY, imageWidth);
                }

                inline ColorProxyT operator*()
                {
                    return ColorProxyT( _impl.components() );
                }

                //inline ColorProxyT* operator->()
                //{
                //    return 0;
                //}

                inline ColorPtrT& operator++()
                {
                    _impl.inc();
                    return *this;
                }

                inline ColorPtrT& operator--()
                {
                    _impl.dec();
                    return *this;
                }

                inline ColorPtrT& operator+=(size_t n)
                {
                    _impl.advance(n);
                    return *this;
                }

                inline ColorPtrT& operator-=(size_t n)
                {
                    _impl.rewind(n);
                    return *this;
                }

                inline bool operator==(const ColorPtrT& c) const
                {
                    return _impl.equals( c._impl );
                }

                inline bool operator!=(const ColorPtrT& c) const
                {
                    return _impl.notEquals( c._impl );
                }

            private:
                inline Math::Point currentXYPosition() const
                {
                    ComponentT* const* components = _impl.components();
                    const size_t pos = components[0] - _beginPlanes;
                    return Math::Point(pos/_imgH, pos/_imgW);
                }

            private:
                //! @brief Image width
                size_t _imgW;

                //! @brief Image height
                size_t _imgH;

                //! @brief Master channel data
                ComponentT* _beginPlanes;

                //! @brief Implementation of the pointer to color
                ColorPtrImpl<ComponentT, NumberOfChannels> _impl;
        };


        //
        // ConstColorPtrT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ConstColorPtrT {
            public:
                inline ConstColorPtrT(const void*)
                : _imgW(0), _imgH(0), _chnStart(0)
                {}

                inline ConstColorPtrT(ConstPixelData& chanPtr,
                                      size_t               imageWidth,
                                      size_t               imageHeight,
                                      size_t               posX,
                                      size_t               posY)
                : _imgW(imageWidth), _imgH(imageHeight), _chnStart(0)
                {
                    _chnStart = chanPtr[0]; // Channel #0 is always the master channel

                    const size_t pos = posY*imageWidth + posX;
                    addElements< NumberOfChannels >(_chnCur, chanPtr, pos);
                }

                inline const ValueT operator*() const
                { return ValueT(_chnCur); }

                inline ConstColorPtrT& operator++()
                {
                    incrementElements<NumberOfChannels>(_chnCur);
                    return *this;
                }

                inline ConstColorPtrT& operator--()
                {
                    decrementElements< NumberOfChannels >(_chnCur);
                    return *this;
                }

                inline ConstColorPtrT& operator+=(size_t n)
                {
                    addAssignElements< NumberOfChannels >(_chnCur, n);
                    return *this;
                }

                inline ConstColorPtrT& operator-=(size_t n)
                {
                    subAssignElements< NumberOfChannels >(_chnCur, n);
                    return *this;
                }

                inline bool operator==(const ConstColorPtrT& c) const
                { return equalElements< NumberOfChannels >(_chnCur, c._chnCur); }

                inline bool operator!=(const ConstColorPtrT& c) const
                { return notEqualElements< NumberOfChannels >(_chnCur, c._chnCur); }

                friend class PlanarImage<PlanarImageModel>;

            private:
                inline Math::Point currentXYPosition() const
                {
                    const size_t pos = _chnCur[0] - _chnStart;
                    return Math::Point(pos/_imgH, pos/_imgW);
                }

            private:
                size_t _imgW; // Image's width

                size_t _imgH; // Image's height

                const ComponentT*   _chnStart; // Start pointer of the master channel

                ConstPixelData _chnCur;   // Pointer to current positions in the channes

                //! @brief Implementation of the pointer to color
                ColorPtrImpl<const ComponentT, NumberOfChannels> _impl;
        };


        //
        // ScanlineT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ScanlineT
        {
            public:
                inline ScanlineT(PixelData& chanPtr,
                                 size_t          imageWidth,
                                 size_t          posY)
                {
                    const size_t pos = posY * imageWidth;
                    addElements< NumberOfChannels >(_chnCur, chanPtr, pos);
                }

                inline ColorProxyT operator[](size_t x)
                { return ColorProxyT(_chnCur, x); }

            private:
                PixelData _chnCur;
        };


        //
        // ConstScanlineT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ConstScanlineT {
            public:
                inline ConstScanlineT(ConstPixelData& chanPtr,
                                      size_t               imageWidth,
                                      size_t               posY)
                {
                    const size_t pos = posY * imageWidth;
                    addElements< NumberOfChannels >(_chnCur, chanPtr, pos);
                }

                inline const ValueT operator[](size_t x) const
                { return ValueT(_chnCur, x); }

            private:
                ConstPixelData _chnCur;
        };

    } // namespace Gfx

} // namespace Pt

#endif

