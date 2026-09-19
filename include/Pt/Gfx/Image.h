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

#ifndef PT_GFX_IMAGE_H
#define PT_GFX_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/View.h>
#include <Pt/Gfx/PixelView.h>
#include <Pt/Gfx/LineView.h>
#include <Pt/Gfx/ImageTraits.h>
#include <Pt/Types.h>
#include <vector>
#include <memory>

namespace Pt {

namespace Gfx {

//inline namespace v2 {

/** @brief Image that owns or wraps pixel data.

    %BasicImage is the image type both families use. A typed image such
    as %Argb32Image is this template with a concrete format. %Image is
    this template with %ImageFormat, so the format is chosen at run
    time. Width, height, and stride come from %ViewBase.

    Constructors that take width and height allocate a buffer the
    image owns. Constructors that take a data pointer wrap that
    buffer without copying it; the caller keeps the memory valid for
    the lifetime of the image. The optional padding is extra bytes
    after each row. %reset() repeats either construction. %clear()
    releases an owned buffer or drops a wrap. Copying an owning
    image copies the pixels. Copying a wrapped image copies the
    pointer, not the pixels, and does not take ownership.

    %data() is the first byte of the first row. %format() is the
    format used to interpret those bytes. Use the free functions
    %view(), %pixelView(), and %lineView() to look at a region
    without copying.

    The example allocates an ARGB-32 image and wraps the same
    dimensions over an external buffer. The second image does not
    own @a bits.

    @code
    Pt::Gfx::Argb32Image owned(64, 48);
    std::vector<Pt::uint8_t> bits(64 * 48 * 4);
    Pt::Gfx::Argb32Image wrapped(bits.data(), 64, 48);
    @endcode

    @ingroup Pt-Gfx-Images
*/
template <typename FormatT, typename TraitsT>
class BasicImage : public ViewBase
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

    public:
        /** @brief Constructs an empty image.
        */
        explicit BasicImage( const Format& format = FormatT::get() );

        /** @brief Constructs an image of the given size with row padding.
        */
        BasicImage(Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding, 
                   const Format& format = FormatT::get() );
        
        /** @brief Constructs an image of the given size.
        */
        BasicImage(Pt::ssize_t width, Pt::ssize_t height, 
                   const Format& format = FormatT::get() );
        
        /** @brief Constructs an image over external pixel data with row padding.
        */
        BasicImage(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding, const Format& format = FormatT::get() );
        
        /** @brief Constructs an image over external pixel data.
        */
        BasicImage(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                   const Format& format = FormatT::get() );
        
        /** @brief Copy constructor.
        */
        BasicImage(const BasicImage& image);
        
        /** @brief Destructor.
        */
        virtual ~BasicImage();

        BasicImage& operator=(const BasicImage& image);

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

        Pt::ssize_t padding() const
        { return stride() - width() * pixelStride(); }
        
        /** @brief Reset to new size.
        */
        void reset(Pt::ssize_t width, Pt::ssize_t height,
               Pt::ssize_t padding, const Format& format);

        void reset(Pt::ssize_t width, Pt::ssize_t height,
               const Format& format);

        /** @brief Reset to new size, keep existing format.
        */
        void reset(Pt::ssize_t width, Pt::ssize_t height,
               Pt::ssize_t padding);

        void reset(Pt::ssize_t width, Pt::ssize_t height);
        
        /** @brief Reset to new data.
        */
        void reset(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
               Pt::ssize_t padding, const Format& format);

        void reset(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
               const Format& format);

        /** @brief Reset to new data, keep existing format.
        */
        void reset(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
               Pt::ssize_t padding);

        void reset(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height);
        
        /** @brief Clears the image.
        */
        void clear();

        Pt::ssize_t pixelStride() const;

        std::size_t size() const;

    private:
        std::unique_ptr<FormatT> _format;
        std::vector<Pt::uint8_t> _buffer;
        Pt::uint8_t* _data;
};

/** @brief Read-only wrap of pixel data.

    %BasicConstImage refers to existing pixels and never allocates.
    Construct it from a const buffer, from a %BasicImage, or from
    another const image. The source buffer must outlive this object.

    @ingroup Pt-Gfx-Images
*/
template <typename FormatT, typename TraitsT>
class BasicConstImage : public ViewBase
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

    public:
        explicit BasicConstImage( const Format& format = FormatT::get() );

        BasicConstImage(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                        Pt::ssize_t padding, const Format& format = FormatT::get() );

        BasicConstImage(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                        const Format& format = FormatT::get() );

        BasicConstImage(const BasicImage<FormatT, TraitsT>& image);

        BasicConstImage(const BasicConstImage& image);

        virtual ~BasicConstImage();

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

        Pt::ssize_t padding() const
        { return stride() - width() * pixelStride(); }

        void reset(const BasicConstImage& image);

        void reset(const BasicImage<FormatT, TraitsT>& image);

        /** @brief Reset to new data.
        */
        void reset(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
               Pt::ssize_t padding, const Format& format);

        void reset(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
               const Format& format);

        /** @brief Reset to new data, keep existing format.
        */
        void reset(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
               Pt::ssize_t padding);

        void reset(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height);

        void clear();

        Pt::ssize_t pixelStride() const;

        std::size_t size() const;

    private:
        std::unique_ptr<FormatT> _format;
        const Pt::uint8_t* _data;
};

// } // namespace

} // namespace

} // namespace

#include <Pt/Gfx/Image.hpp>

#endif
