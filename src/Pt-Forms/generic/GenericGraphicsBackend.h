/* Copyright (C) 2015-2026 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#ifndef PT_FORMS_GENERIC_GRAPHICSBACKEND_H
#define PT_FORMS_GENERIC_GRAPHICSBACKEND_H

#include "GenericPixmapImpl.h"

#include <Pt/Forms/GraphicsBackend.h>
#include <Pt/Gfx/Bitmap.h>

namespace Pt {

namespace Forms {

class GenericGraphicsBackend : public GraphicsBackend
{
    public:
        IPixmapImpl* createPixmapImpl() override
        {
            return new GenericPixmapImpl();
        }

        const std::string& defaultFont() const override
        {
            return Gfx::Bitmap::defaultFont();
        }

        void setDefaultFont(const std::string& family) override
        {
            Gfx::Bitmap::setDefaultFont(family);
        }

        std::vector<std::string> fontFamilies() const override
        {
            return Gfx::Bitmap::fontFamilies();
        }

        std::vector<Gfx::FontFace> fontFaces(const std::string& family) const override
        {
            return Gfx::Bitmap::fontFaces(family);
        }

        const Gfx::Image& image(const Pixmap& pixmap) const
        {
            const GenericPixmapImpl* impl = static_cast<const GenericPixmapImpl*>( pixmap.impl() );
            return impl->bitmap().image();
        }
};

} // namespace

} // namespace

#endif