/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef Pt_Hmi_Picture_h
#define Pt_Hmi_Picture_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Painter.h>
#include <cstddef>

namespace Pt {

namespace Hmi {

class PictureImpl;

class PT_HMI_API Picture
{
    public:
        Picture();

        /** @brief Construct without mask using the alpha channel of the image.
        */
        Picture(const Gfx::Image& image);

        /** @brief Construct with mask derived from the alpha channel of the image.

            Each pixel with an alpha value below @a alphaThreshold will be
            masked and each pixel with an alpha value above will be drawn 
            as if its alpha value is 1.
        */
        Picture(const Gfx::Image& image, float alphaThreshold);
        
        ~Picture();

        Picture(const Picture& p);

        Picture& operator=(const Picture& p);

        void set(const Gfx::Image& image);

        bool empty() const;

        std::size_t width() const;

        std::size_t height() const;

        PictureImpl* impl();

        const PictureImpl* impl() const;

    private:
        PictureImpl* _impl;
};

} // namespace

} // namespace

#endif