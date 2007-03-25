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
#ifndef Pt_Gfx_PlanarImageView1x1_tpp
#define Pt_Gfx_PlanarImageView1x1_tpp


namespace Pt {

    namespace Gfx {

        //
        // PlanarImageView<ColorProxyT_, 1, 1> static constants definition
        //
        //template<typename ColorModelT>
        //const size_t PlanarImageView<ColorModelT, 1, 1>::NumberOfChannels;


        //
        // PlanarImageView<ColorProxyT_, 1, 1> member functions' implementation
        //
        //template<typename ColorProxyT_>
        //inline void PlanarImageView<ColorProxyT_, 1, 1>::alloc(size_t imageWidth, size_t imageHeight)
        //{
            // For subsampled planar image, these values won't be
            // this easy to calculate
            //const size_t planeSize = imageWidth * imageHeight * sizeof(ComponentT);
            //const size_t imageSize = NumberOfChannels * planeSize;

            // _buff.resize(imageSize);

            // _chanPtr[0] = &_buff[0];
            // for(size_t i = 1; i < NumberOfChannels; ++i)
            //     _chanPtr[i] = _chanPtr[i-1] + planeSize;

            // _width  = imageWidth;
            // _height = imageHeight;
        //}


        template<typename ColorModelT>
        inline Pt::size_t PlanarImageView<ColorModelT, 1, 1>::size(size_t width, size_t height)
        {
            const size_t planeSize = width * height * sizeof(Component);
            const size_t imageSize = ColorModelT::NumberOfChannels * planeSize;
            return imageSize;
        }


        template<typename ColorModelT>
        inline void PlanarImageView<ColorModelT, 1, 1>::init(unsigned char* memory, size_t width, size_t height)
        {
            const size_t planeSize = width * height * sizeof(Component);

            _chanPtr[0] = reinterpret_cast<Component*>(memory);
            for(size_t i = 1; i < ColorModelT::NumberOfChannels; ++i)
                _chanPtr[i] = _chanPtr[i-1] + planeSize;

            _width  = width;
            _height = height;
        }


        //
        // ColorPtrT
        //
        template<typename ColorModelT>
        class PlanarImageView<ColorModelT, 1, 1>::ColorPtrT
        {
            public:
                inline ColorPtrT(void*)
                : _imageWidth(0), _imageHeight(0), _chn0Start(0)
                {}

                // NOTE: Later if the PlanarImage<T> class has been implemented, this
                //       function should take a reference to the image instead of 3
                //       separated parameters (chnDataStart, imageWidth and imageHeight)
                inline ColorPtrT(ColorData& chnDataStart,
                                 size_t       imageWidth,
                                 size_t       imageHeight,
                                 size_t       posX,
                                 size_t       posY)
                : _imageWidth(imageWidth), _imageHeight(imageHeight), _chn0Start(0)
                {
                    _chn0Start = chnDataStart[0]; // Convention: channel #0 is always
                                                  //             the master channel

                    size_t pos = posY*imageWidth + posX;
                    addElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, chnDataStart, pos);
                }

                inline ColorRef operator*()
                { return ColorRef(_chnDataCur); }

                inline const ColorRef operator*() const
                { return ColorRef(_chnDataCur); }

                inline ColorRef operator[](size_t offset)
                { return ColorRef(_chnDataCur, offset); }

                inline const ColorRef operator[](size_t offset) const
                { return ColorRef(_chnDataCur, offset); }

                inline ColorPtrT& operator++()
                {
                    incrementElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur);
                    return *this;
                }

                inline ColorPtrT& operator--()
                {
                    decrementElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur);
                    return *this;
                }

                inline ColorPtrT& operator+=(size_t n)
                {
                    addAssignElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, n);
                    return *this;
                }

                inline ColorPtrT& operator-=(size_t n)
                {
                    subAssignElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, n);
                    return *this;
                }

                bool operator==(const ColorPtrT& c) const
                { return equalElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, c._chnDataCur); }

                bool operator!=(const ColorPtrT& c) const
                { return notEqualElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, c._chnDataCur); }

                friend class PlanarImage<PlanarImageView>;

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
                Component* _chn0Start;

                // Pointers to the current data of all channels
                // (the pixel at the current (x,y) coordinate)
                ColorData _chnDataCur;
        };


        //
        // ConstColorPtrT
        //
        template<typename ColorModelT>
        class PlanarImageView<ColorModelT, 1, 1>::ConstColorPtrT
        {
            public:
                inline ConstColorPtrT(const void*)
                : _imageWidth(0), _imageHeight(0), _chn0Start(0)
                {}

                // NOTE: Later if the PlanarImage<T> class has been implemented, this
                //       function should take a constant reference to the image instead of
                //       3 separated parameters (chnDataStart, imageWidth and imageHeight)
                inline ConstColorPtrT(ConstColorData& chnDataStart,
                                      size_t            imageWidth,
                                      size_t            imageHeight,
                                      size_t            posX,
                                      size_t            posY)
                : _imageWidth(imageWidth), _imageHeight(imageHeight), _chn0Start(0)
                {
                    _chn0Start = chnDataStart[0]; // Convention: channel #0 is always
                                                  //             the master channel

                    const size_t pos = posY*imageWidth + posX;
                    addElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, chnDataStart, pos);
                }

                inline const Color operator*() const
                { return ValueT(_chnDataCur); }

                inline const Color operator[](size_t offset) const
                { return ValueT(_chnDataCur, offset); }

                inline ConstColorPtrT& operator++()
                {
                    incrementElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur);
                    return *this;
                }

                inline ConstColorPtrT& operator--()
                {
                    decrementElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur);
                    return *this;
                }

                inline ConstColorPtrT& operator+=(size_t n)
                {
                    addAssignElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, n);
                    return *this;
                }

                inline ConstColorPtrT& operator-=(size_t n)
                {
                    subAssignElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, n);
                    return *this;
                }

                bool operator==(const ConstColorPtrT& c) const
                { return equalElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, c._chnDataCur); }

                bool operator!=(const ConstColorPtrT& c) const
                { return notEqualElements< ColorModelT::NumberOfChannels, 0 >(_chnDataCur, c._chnDataCur); }

                friend class PlanarImage<PlanarImageView>;

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
                const Component* _chn0Start;

                // Pointers to the current data of all channels
                // (the pixel at the current (x,y) coordinate)
                ConstColorData  _chnDataCur;
        };


        //
        // ScanlineT
        //
/*
        template<typename ColorProxyT_>
        class PlanarImageView<ColorProxyT_, 1, 1>::ScanlineT
        {
            public:
                inline ScanlineT(ChannelData& chnDataStart,
                                 size_t       imageWidth,
                                 size_t       posY)
                {
                    const size_t pos = posY * imageWidth;
                    addElements< NumberOfChannels, 0 >(_chnDataCur, chnDataStart, pos);
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
*/

        //
        // ConstScanlineT
        //

/*
        template<typename ColorProxyT_>
        class PlanarImageView<ColorProxyT_, 1, 1>::ConstScanlineT {
            public:
                inline ConstScanlineT(ConstChannelData& chnDataStart,
                                      size_t            imageWidth,
                                      size_t            posY)
                {
                    const size_t pos = posY * imageWidth;
                    addElements< NumberOfChannels, 0 >(_chnDataCur, chnDataStart, pos);
                }

                inline const ValueT operator[](size_t x) const
                { return ValueT(_chnDataCur, x); }

            private:
                // Pointers to the current data of all channels
                // (the pixel at the current (0,y) coordinate)
                ConstChannelData _chnDataCur;
        };
*/

    } // namespace Gfx

} // namespace Pt

#endif

