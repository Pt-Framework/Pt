/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_GFX_IMAGE_VIEW_H
#define PT_GFX_IMAGE_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageModel.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

/** @brief View on image data.
*/
class ImageView : public BasicView<ImageModel>
{
    public:
        ImageView()
        : BasicView()
        , _model( ImageFormat::argb32() )
        { }

        explicit ImageView(const ImageFormat& format)
        : BasicView()
        , _model(format)
        { }

        ImageView(const ImageFormat& format, Pt::uint8_t* data,
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
        : BasicView()
        , _model(format)
        { 
            reset(_model, data, width, height, padding);
        }

        virtual ~ImageView()
        { }

        const ImageFormat& format() const
        { return _model.format(); }

    private:
        ImageModel _model;
};

} // namespace

} // namespace

#endif
