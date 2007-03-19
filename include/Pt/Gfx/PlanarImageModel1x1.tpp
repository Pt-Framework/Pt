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

        //
        // Static constants definition
        //
        template<typename ColorProxyT_>
        const size_t PlanarImageModel<ColorProxyT_, 1, 1>::NumberOfChannels;


        //
        // ColorPtrT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ColorPtrT
        {
            public:
                inline ColorPtrT(void*)
                : _imageWidth(0), _imageHeight(0), _chn0Start(0)
                {}

                // NOTE: Later if the PlanarImage<T> class has been implemented, this
                //       function should take a reference to the image instead of 3
                //       separated parameters (chnDataStart, imageWidth and imageHeight)
                inline ColorPtrT(ChannelData& chnDataStart,
                                 size_t       imageWidth,
                                 size_t       imageHeight,
                                 size_t       posX,
                                 size_t       posY)
                : _imageWidth(imageWidth), _imageHeight(imageHeight), _chn0Start(0)
                {
                    _chn0Start = chnDataStart[0]; // Convention: channel #0 is always
                                                  //             the master channel

                    size_t pos = posY*imageWidth + posX;
                    addElements< NumberOfChannels >(_chnDataCur, chnDataStart, pos);
                }

                inline ProxyT operator*()
                { return ProxyT(_chnDataCur); }

                inline const ProxyT operator*() const
                { return ProxyT(_chnDataCur); }

                inline ProxyT operator[](size_t offset)
                { return ProxyT(_chnDataCur, offset); }

                inline const ProxyT operator[](size_t offset) const
                { return ProxyT(_chnDataCur, offset); }

                inline ColorPtrT& operator++()
                {
                    incrementElements<NumberOfChannels>(_chnDataCur);
                    return *this;
                }

                inline ColorPtrT& operator--()
                {
                    decrementElements< NumberOfChannels >(_chnDataCur);
                    return *this;
                }

                inline ColorPtrT& operator+=(size_t n)
                {
                    addAssignElements< NumberOfChannels >(_chnDataCur, n);
                    return *this;
                }

                inline ColorPtrT& operator-=(size_t n)
                {
                    subAssignElements< NumberOfChannels >(_chnDataCur, n);
                    return *this;
                }

                bool operator==(const ColorPtrT& c) const
                { return equalElements< NumberOfChannels >(_chnDataCur, c._chnDataCur); }

                bool operator!=(const ColorPtrT& c) const
                { return notEqualElements< NumberOfChannels >(_chnDataCur, c._chnDataCur); }

                friend class PlanarImage<PlanarImageModel>;

            private:
                inline const Math::Point currentXYPosition() const
                {
                    size_t pos = _chnDataCur[0] - _chn0Start;
                    return Math::Point(pos/_imageHeight, pos/_imageWidth);
                }

            private:
                size_t _imageWidth;
                size_t _imageHeight;

                // Pointer to the start of data of channel #0 (master channel)
                ComponentT* _chn0Start;

                // Pointers to the current data of all channels
                // (the pixel at the current (x,y) coordinate)
                ChannelData _chnDataCur;
        };


        //
        // ConstColorPtrT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ConstColorPtrT
        {
            public:
                inline ConstColorPtrT(const void*)
                : _imageWidth(0), _imageHeight(0), _chn0Start(0)
                {}

                // NOTE: Later if the PlanarImage<T> class has been implemented, this
                //       function should take a constant reference to the image instead of
                //       3 separated parameters (chnDataStart, imageWidth and imageHeight)
                inline ConstColorPtrT(ConstChannelData& chnDataStart,
                                      size_t            imageWidth,
                                      size_t            imageHeight,
                                      size_t            posX,
                                      size_t            posY)
                : _imageWidth(imageWidth), _imageHeight(imageHeight), _chn0Start(0)
                {
                    _chn0Start = chnDataStart[0]; // Convention: channel #0 is always
                                                  //             the master channel

                    const size_t pos = posY*imageWidth + posX;
                    addElements< NumberOfChannels >(_chnDataCur, chnDataStart, pos);
                }

                inline const ValueT operator*() const
                { return ValueT(_chnDataCur); }

                inline const ValueT operator[](size_t offset) const
                { return ValueT(_chnDataCur, offset); }

                inline ConstColorPtrT& operator++()
                {
                    incrementElements<NumberOfChannels>(_chnDataCur);
                    return *this;
                }

                inline ConstColorPtrT& operator--()
                {
                    decrementElements< NumberOfChannels >(_chnDataCur);
                    return *this;
                }

                inline ConstColorPtrT& operator+=(size_t n)
                {
                    addAssignElements< NumberOfChannels >(_chnDataCur, n);
                    return *this;
                }

                inline ConstColorPtrT& operator-=(size_t n)
                {
                    subAssignElements< NumberOfChannels >(_chnDataCur, n);
                    return *this;
                }

                bool operator==(const ConstColorPtrT& c) const
                { return equalElements< NumberOfChannels >(_chnDataCur, c._chnDataCur); }

                bool operator!=(const ConstColorPtrT& c) const
                { return notEqualElements< NumberOfChannels >(_chnDataCur, c._chnDataCur); }

                friend class PlanarImage<PlanarImageModel>;

            private:
                inline const Math::Point currentXYPosition() const
                {
                    const size_t pos = _chnDataCur[0] - _chn0Start;
                    return Math::Point(pos/_imageHeight, pos/_imageWidth);
                }

            private:
                size_t _imageWidth;
                size_t _imageHeight;

                // Pointer to the start of data of channel #0 (master channel)
                const ComponentT* _chn0Start;

                // Pointers to the current data of all channels
                // (the pixel at the current (x,y) coordinate)
                ConstChannelData  _chnDataCur;
        };


        //
        // ScanlineT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ScanlineT
        {
            public:
                inline ScanlineT(ChannelData& chnDataStart,
                                 size_t       imageWidth,
                                 size_t       posY)
                {
                    const size_t pos = posY * imageWidth;
                    addElements< NumberOfChannels >(_chnDataCur, chnDataStart, pos);
                }

                inline ProxyT operator[](size_t x)
                { return ProxyT(_chnDataCur, x); }

                inline const ProxyT operator[](size_t x) const
                { return ProxyT(_chnDataCur, x); }

            private:
                // Pointers to the current data of all channels
                // (the pixel at the current (0,y) coordinate)
                ChannelData _chnDataCur;
        };


        //
        // ConstScanlineT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ConstScanlineT {
            public:
                inline ConstScanlineT(ConstChannelData& chnDataStart,
                                      size_t            imageWidth,
                                      size_t            posY)
                {
                    const size_t pos = posY * imageWidth;
                    addElements< NumberOfChannels >(_chnDataCur, chnDataStart, pos);
                }

                inline const ValueT operator[](size_t x) const
                { return ValueT(_chnDataCur, x); }

            private:
                // Pointers to the current data of all channels
                // (the pixel at the current (0,y) coordinate)
                ConstChannelData _chnDataCur;
        };

    } // namespace Gfx

} // namespace Pt

#endif

