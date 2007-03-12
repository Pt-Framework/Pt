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
#ifndef Pt_Gfx_PlanarImageModel1x1_tpp
#define Pt_Gfx_PlanarImageModel1x1_tpp


namespace Pt {

    namespace Gfx {

        template<typename ColorProxyT_>
        const size_t PlanarImageModel<ColorProxyT_, 1, 1>::NumberOfChannels;


        //
        // ColorPtrT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ColorPtrT {
            public:
                inline ColorPtrT(void*)
                : _imgW(0), _imgH(0), _chnStart(0)
                {}

                inline ColorPtrT(std::vector<ComponentT*>& chanPtr,
                                 size_t                    imageWidth,
                                 size_t                    imageHeight,
                                 size_t                    posX,
                                 size_t                    posY)
                : _imgW(imageWidth), _imgH(imageHeight), _chnStart(0)
                {
                    const size_t pos = posY*imageWidth + posX;

                    _chnStart = chanPtr[0]; // Channel #0 is always the master channel

                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        _chnCur[i] = chanPtr[i] + pos;
                }

                inline ColorProxyT operator*()
                { return ColorProxyT(_chnCur); }

                inline ColorPtrT& operator++()
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) ++_chnCur[i];
                    return *this;
                }

                inline ColorPtrT& operator--()
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) --_chnCur[i];
                    return *this;
                }

                inline ColorPtrT& operator+=(size_t n)
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) _chnCur[i] += n;
                    return *this;
                }

                inline ColorPtrT& operator-=(size_t n)
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) _chnCur[i] -= n;
                    return *this;
                }

                inline bool operator==(const ColorPtrT& c) const
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) {
                        if(_chnCur[i] != c._chnCur[i]) return(false);
                    }
                    return true;
                }

                inline bool operator!=(const ColorPtrT& c) const
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) {
                        if(_chnCur[i] == c._chnCur[i]) return(false);
                    }
                    return true;
                }

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

                ComponentT*              _chnStart; // Start pointer of the master channel
                std::vector<ComponentT*> _chnCur;   // Pointer to current positions in the channes
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

                inline ConstColorPtrT(const std::vector<const ComponentT*>& chanPtr,
                                      size_t                                imageWidth,
                                      size_t                                imageHeight,
                                      size_t                                posX,
                                      size_t                                posY)
                : _imgW(imageWidth), _imgH(imageHeight), _chnStart(0)
                {
                    const size_t pos = posY*imageWidth + posX;

                    _chnStart = chanPtr[0]; // Channel #0 is always the master channel

                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        _chnCur[i] = chanPtr[i] + pos;
                }

                inline const ValueT operator*() const
                { return ValueT(_chnCur); }

                inline ConstColorPtrT& operator++()
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) ++_chnCur[i];
                    return *this;
                }

                inline ConstColorPtrT& operator--()
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) --_chnCur[i];
                    return *this;
                }

                inline ConstColorPtrT& operator+=(size_t n)
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) _chnCur[i] += n;
                    return *this;
                }

                inline ConstColorPtrT& operator-=(size_t n)
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) _chnCur[i] -= n;
                    return *this;
                }

                inline bool operator==(const ConstColorPtrT& c) const
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) {
                        if(_chnCur[i] != c._chnCur[i]) return(false);
                    }
                    return true;
                }

                inline bool operator!=(const ConstColorPtrT& c) const
                {
                    for(size_t i = 0; i < NumberOfChannels; ++i) {
                        if(_chnCur[i] == c._chnCur[i]) return(false);
                    }
                    return true;
                }

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

                const ComponentT*              _chnStart; // Start pointer of the master channel
                std::vector<const ComponentT*> _chnCur;   // Pointer to current positions in the channes
        };


        //
        // ScanlineT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ScanlineT {
            public:
                inline ScanlineT(std::vector<ComponentT*>& chanPtr,
                                 size_t                    imageWidth,
                                 size_t                    posY)
                {
                    const size_t pos = posY*imageWidth;

                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        _chnCur[i] = chanPtr[i] + pos;
                }

                inline ColorProxyT operator[](size_t x)
                { return ColorProxyT(_chnCur, x); }

            private:
                std::vector<ComponentT*> _chnCur;
        };


        //
        // ConstScanlineT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ConstScanlineT {
            public:
                inline ConstScanlineT(const std::vector<const ComponentT*>& chanPtr,
                                      size_t                                imageWidth,
                                      size_t                                posY)
                {
                    const size_t pos = posY*imageWidth;

                    for(size_t i = 0; i < NumberOfChannels; ++i)
                        _chnCur[i] = chanPtr[i] + pos;
                }

                inline const ValueT operator[](size_t x) const
                { return ValueT(_chnCur, x); }

            private:
                std::vector<const ComponentT*> _chnCur;
        };

    } // namespace Gfx

} // namespace Pt

#endif

