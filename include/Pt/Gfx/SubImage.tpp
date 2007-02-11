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
#ifndef Pt_Gfx_SubImage_tpp
#define Pt_Gfx_SubImage_tpp


namespace Pt {

    namespace Gfx {

        //
        // SubImage<ImageT_> implementation
        //

        template <typename ImageT_>
        SubImage<ImageT_>::SubImage(ImageT& image, const Region& area)
        : _image(image), _area(area)
        {
            int x1 = area.x();
            int y1 = area.y();
            int x2 = x1 + area.width() - 1;
            int y2 = y1 + area.height() - 1;

            if(x1<0 || y1<0 || x2<0 ||y2<0 ||
               x1>=int(_image.width()) || y1>=int(_image.height()) ||
               x2>=int(_image.width()) || y2>=int(_image.height()))
                throw std::range_error("The given region covers invalid area in the given image " + PT_SOURCEINFO);
        }


        template <typename ImageT_>
        bool SubImage<ImageT_>::operator==(const SubImage& src)
        {
            for(size_t y = 0; y < _area.height(); y++) {
                if( memcmp(scanline(y), src.scanline(y),
                             sizeof(SubImage) * _area.width()) )
                    return false;
            }
            return true;
        }


        template <typename ImageT_>
        SubImage<ImageT_>& SubImage<ImageT_>::operator=(const ColorT& color)
        {
            for(size_t y = 0; y < _area.height(); y++) {
                for(size_t x = 0; x < _area.width(); x++) {
                    _image.scanline(y)[x] = color;
                }
            }
            return *this;
        }

        template <typename ImageT_>
        SubImage<ImageT_>& SubImage<ImageT_>::operator=(const ImageT& src)
        {
            // If the size is not the same, we need to scale it first then copy
            if(_area.width()!=src.width() || _area.height()!=src.height()) {
        ImageT tmp(_area.width(), _area.height());
        blockScale(src.begin(), src.end(), tmp.begin(), tmp.end()); // TODO: Later, user should be able to
                for(uint y = 0; y < _area.height(); y++)                    //       choose the scalling algorithm
                    for(uint x = 0; x < _area.width(); x++)
                        _image.pixel(x+_area.x(), y+_area.y()) = tmp.pixel(x, y); // TODO: Optimize it !!!
            }
            // If the size is the same, we just need to copy
            else {
                for(uint y = 0; y < _area.height(); y++)
                    for(uint x = 0; x < _area.width(); x++)
                        _image.pixel(x+_area.x(), y+_area.y()) = src.pixel(x, y); // TODO: Optimize it !!!
            }
            return *this;
        }

        template <typename ImageT_>
        SubImage<ImageT_>& SubImage<ImageT_>::operator=(const SubImage& src)
        {
            // If the size is not the same, we need to scale it first then copy
            if(_area.width()!=src.width() || _area.height()!=src.height()) {
                // Copy the source pixels to a temporary image
                ImageT tmp1(src.width(), src.height());
                for(uint y = 0; y < src.height(); y++)
                    for(uint x = 0; x < src.width(); x++)
                        tmp1.pixel(x, y) = src._image.pixel(x+src._area.x(), y+src._area.y()); // TODO: Optimize it !!!
                // Scale the temporary image to another temporary image
        ImageT tmp2(_area.width(), _area.height());
        blockScale(tmp1.begin(), tmp1.end(), tmp2.begin(), tmp2.end()); // TODO: Later, user should be able to
                                                                                //       choose the scalling algorithm
                // Copy the pixels to this sub image
                for(uint y = 0; y < _area.height(); y++)
                    for(uint x = 0; x < _area.width(); x++)
                        _image.pixel(x+_area.x(), y+_area.y()) = tmp2.pixel(x, y); // TODO: Optimize it !!!
            }
            // If the size is the same, we just need to copy convert
            else {
                // If the source and destination image are the same
                // we must use temporary image
                if(&_image == &src._image) {
                    // Copy the source pixels to the temporary image
                    ImageT tmp(_area.width(), _area.height());
                    for(uint y = 0; y < src.height(); y++)
                        for(uint x = 0; x < src.width(); x++)
                            tmp.pixel(x, y) = src._image.pixel(x+src._area.x(), y+src._area.y()); // TODO: Optimize it !!!
                    // Copy the pixels to this sub image
                    for(uint y = 0; y < _area.height(); y++)
                        for(uint x = 0; x < _area.width(); x++)
                            _image.pixel(x+_area.x(), y+_area.y()) = tmp.pixel(x, y); // TODO: Optimize it !!!
                }
                // If the source and destination image are different, just copy
                else {
                    for(uint y = 0; y < _area.height(); y++)
                        for(uint x = 0; x < _area.width(); x++)
                            _image.pixel(x+_area.x(), y+_area.y()) =
                                    src._image.pixel(x+src._area.x(), y+src._area.y()); // TODO: Optimize it !!!
                }
            }
            return *this;
        }


        template <typename ImageT_>
        typename SubImage<ImageT_>::ColorT& SubImage<ImageT_>::at(int x, int y)
        {
            if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height()))
        throw std::range_error("Either the image is empty or the (y,x) coordinate is invalid" + PT_SOURCEINFO);
            return _image.data()[(y+_area.y())*_image.width() + x+_area.x()];
        }


        template <typename ImageT_>
        const typename SubImage<ImageT_>::ColorT& SubImage<ImageT_>::at(int x, int y) const
        {
            if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height()))
        throw std::range_error("Either the image is empty or the (y,x) coordinate is invalid" + PT_SOURCEINFO);
            return _image.data()[(y+_area.y())*_image.width() + x+_area.x()];
        }


        template <typename ImageT_>
        const typename SubImage<ImageT_>::ColorT& SubImage<ImageT_>::color(int x, int y, const ColorT& invalid) const
        {
            if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height())) return invalid;
            return _image.data()[(y+_area.y())*_image.width() + x+_area.x()];
        }


        template <typename ImageT_>
        void SubImage<ImageT_>::setColor(int x, int y, const ColorT& color_)
        {
            if(_image.empty() || x<0 || x>=int(_area.width()) || y<0 || y>int(_area.height())) return;
            _image.data()[(y+_area.y())*_image.width() + x+_area.x()] = color_;
        }

    } // namespace Gfx

} // namespace Pt

#endif
