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
#ifndef Pt_Gfx_PlanarImage_tpp
#define Pt_Gfx_PlanarImage_tpp


namespace Pt {

    namespace Gfx {

#if 0

        //
        // PlanarImage<ColorProxyT> implementation
        //

        template <typename ColorProxyT_, typename ColorTraitsT_>
        void PlanarImage<ColorProxyT_, ColorTraitsT_>::resize(uint width_, uint height_)
        {
            std::vector<size_t> _chanTSize;

            size_t totalBufferSize = 0;

            // Calculate the total image buffer size and the size of each channel
            for(size_t i = 0; i < ColorTraitsT::ChannelCount; ++i) {
                // Get the subsampling factors
                const size_t ssX = ColorTraitsT::ChannelSubsamplingX(i);
                const size_t ssY = ColorTraitsT::ChannelSubsamplingY(i);
                // Calculate the real channel size
                const size_t szX = (ssX > 1) ? size_t(float(width_)  / float(ssX) + 0.5f) : width_;
                const size_t szY = (ssY > 1) ? size_t(float(height_) / float(ssY) + 0.5f) : height_;
                const size_t szI = szX * szY;
                // Store the size in a temporary variable
                _chanTSize.push_back(szI);
                // Add the size to the total image size
                totalBufferSize += szI;
            }

            // Resize the buffers
            _buff.resize(totalBufferSize);
            _chanPtr.resize(ColorTraitsT::ChannelCount);
            _chanSize.resize(ColorTraitsT::ChannelCount);

            // Calculate the start position of each channel in the buffer
            // and also save the channel's size to the dedicated variable
            _chanPtr[0]  = &_buff[0];
            _chanSize[0] = _chanTSize[0];
            for(size_t i = 1; i < ColorTraitsT::ChannelCount; ++i) {
                _chanPtr[i]  = _chanPtr[i - 1] + _chanTSize[i - 1];
                _chanSize[i] = _chanTSize[i];
            }

            _width  = width_;
            _height = height_;
        }

        template <typename ColorProxyT_, typename ColorTraitsT_>
        void PlanarImage<ColorProxyT_, ColorTraitsT_>::resize(uint width_, uint height_, const ColorT& fill)
        {
            resize(width_, height_);
            *this = fill;
        }


        template <typename ColorProxyT_, typename ColorTraitsT_>
        PlanarImage<ColorProxyT_, ColorTraitsT_>& PlanarImage<ColorProxyT_, ColorTraitsT_>::operator=(const ColorT& fill)
        {
            ColorPtrT ptr(_chanPtr, _width, _height, 0, 0);

            for(size_t i = 0; i < _width*_height; ++i) {
                *ptr = fill;
                ++ptr;
            }

            return *this;
        }

        template <typename ColorProxyT_, typename ColorTraitsT_>
        PlanarImage<ColorProxyT_, ColorTraitsT_>& PlanarImage<ColorProxyT_, ColorTraitsT_>::operator=(const PlanarImage& src)
        {
            if(src.empty()) {
                clear();
                return *this;
            }

            if(_width!=src._width || _height!=src._height) resize(src._width, src._height);

            memcpy(&_buff[0], &src._buff[0], _buff.size() * sizeof(ComponentT));

            return *this;
        }


        template <typename ColorProxyT_, typename ColorTraitsT_>
        typename PlanarImage<ColorProxyT_, ColorTraitsT_>::ColorProxyT PlanarImage<ColorProxyT_, ColorTraitsT_>::at(int x, int y)
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height))
                throw std::range_error("Either the image is empty or the (y,x) coordinate is invalid" + PT_SOURCEINFO);

            return *ColorPtrT(_chanPtr, _width, _height, x, y);
        }

        template <typename ColorProxyT_, typename ColorTraitsT_>
        const typename PlanarImage<ColorProxyT_, ColorTraitsT_>::ColorT PlanarImage<ColorProxyT_, ColorTraitsT_>::at(int x, int y) const
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height))
                throw std::range_error("Either the image is empty or the (y,x) coordinate is invalid" + PT_SOURCEINFO);

            ColorT col;
            assign(col, *ConstColorPtrT(_chanPtr, _width, _height, x, y));
            return col;
        }

        template <typename ColorProxyT_, typename ColorTraitsT_>
        const typename PlanarImage<ColorProxyT_, ColorTraitsT_>::ColorT PlanarImage<ColorProxyT_, ColorTraitsT_>::color(int x, int y, const ColorT& invalid) const
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height)) return invalid;

            ColorT col;
            assign(col, *ConstColorPtrT(_chanPtr, _width, _height, x, y));
            return col;
        }

        template <typename ColorProxyT_, typename ColorTraitsT_>
        void PlanarImage<ColorProxyT_, ColorTraitsT_>::setColor(int x, int y, const ColorT& color_)
        {
            if(empty() || x<0 || x>=int(_width) || y<0 || y>=int(_height)) return;

            *ColorPtrT(_chanPtr, _width, _height, x, y) = color_;
        }

#endif

    } // namespace Gfx

} // namespace Pt

#endif

