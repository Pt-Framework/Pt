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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_API_IMAGES_H
#define PT_GFX_API_IMAGES_H

/** @addtogroup Pt-Gfx-Images

    @brief Images, formats, views and pixels.

    An image is a rectangular pixel buffer. %BasicImage either owns that
    buffer or wraps a caller-supplied one. Constructors that take width
    and height allocate. Constructors that take a data pointer wrap;
    the buffer must cover the given size and padding and must remain
    valid for the lifetime of the image. %reset() switches between
    those two modes. %clear() drops the buffer. Copying an owning
    image copies the pixels. A wrapped image does not take ownership
    on copy of the pointer; the original buffer still belongs to the
    caller.

    Format is either a compile-time type or a runtime value. A typed
    image binds the format in the template argument: %Argb32Image,
    %Rgb32Image, %Rgb16Image, and %Yuv12Image are %BasicImage
    specializations, and their pixels are the matching concrete types
    such as %Argb32Pixel. Use a typed image when the calling code
    already knows the format. %Image selects the format at runtime
    through %ImageFormat. Decoders, file loaders, and other input that
    discovers a format while running use %Image. %Image exposes
    %Pixel<Color>. %ImageF is not a float storage format: it is a
    %ColorF lens on the same runtime %ImageFormat, and it exposes
    %Pixel<ColorF>. The generic pixels forward storage operations to
    the selected format.

    %Color is the 8-bit working color, stored as a packed ARGB value
    with straight alpha. %ColorF is a four-float working color in
    RGBA order, with channels in the unit interval and values above 1
    allowed for HDR. Neither type is a pixel buffer. Pixels convert
    to and from the working color of their traits.

    Views do not copy pixels. The free functions %view(),
    %pixelView(), and %lineView() build a non-owning region, a
    pixel-iterator range, or a row span over an image or another
    view. Each view keeps the source format, typed or runtime, and
    the source buffer must outlive the view. %ViewBase stores width,
    height, and stride for every image and view.

    A pixel object is a cursor, not a color value. It refers to one
    position in an image or view. Copy construction duplicates the
    cursor. Copy assignment is deleted so a `pixel = other` cannot
    silently rebind the position; call %reset() to bind another
    location. Assigning a %Color or calling %assign() writes through
    to storage. %advance() and %skipPadding() walk the buffer in
    scanline order.

    JPEG and PNG codecs read and write %Image values on iostreams.
    %JpegReader and %PngReader attach a stream and an image, then
    %get() fills that image. %PngWriter attaches an output stream
    and %write() emits the image. The reader owns neither the stream
    nor the image.

    Add a format by deriving it from %ImageFormat, implementing its
    pixel storage operations, and providing the matching concrete
    pixel types and an %ImageTraits specialization. Typed images then
    use the format as a %BasicImage argument, and the runtime image
    API uses it through %ImageFormat.

    The example decodes a JPEG into a runtime %Image and opens a
    crop that does not copy pixels.

    @code
    std::ifstream in("photo.jpg", std::ios::binary);
    Pt::Gfx::Image image;
    Pt::Gfx::JpegReader reader(in, image);
    reader.get();

    Pt::Gfx::ImageView crop = Pt::Gfx::view(image, 10, 10, 64, 64);
    @endcode
*/

#endif
