#ifndef PT_GFX_API_RGB32IMAGE_H
#define PT_GFX_API_RGB32IMAGE_H

#include <Pt/Gfx/Api.h>

namespace Pt {

namespace Gfx {

/** @brief Image storing pixels in premultiplied ARGB-32 format.

    %Rgb32Image owns its pixel data and stores each pixel as a 32-bit value
    with four 8-bit channels in premultiplied ARGB-32 layout. When colors are
    written, the RGB components are multiplied by alpha. This format is the
    native target for compositing operations and avoids the per-pixel division
    during blending.

    @headerfile Rgb32Image.h <Pt/Gfx/Rgb32Image.h>
    @ingroup Pt-Gfx-Images
*/
class Rgb32Image : public BasicImage<Rgb32>
{
    public:
        /** @brief Constructs an empty image.

            Creates an image with no pixel data. The @p format parameter
            selects the pixel format and defaults to %Rgb32::get().
        */
        explicit Rgb32Image(const Rgb32& format = Rgb32::get());

        /** @brief Constructs an image of the given size.

            Allocates pixel data for @p width x @p height pixels. The
            @p format parameter selects the pixel format and defaults to
            %Rgb32::get().
        */
        Rgb32Image(Pt::ssize_t width, Pt::ssize_t height,
                   const Rgb32& format = Rgb32::get());

        /** @brief Constructs an image of the given size with row padding.

            Allocates pixel data for @p width x @p height pixels. Each row
            is extended by @p padding extra bytes beyond the last pixel,
            which is useful when the image data must be aligned to a specific
            stride. The @p format parameter selects the pixel format and
            defaults to %Rgb32::get().
        */
        Rgb32Image(Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding,
                   const Rgb32& format = Rgb32::get());

        /** @brief Constructs an image over external pixel data.

            Wraps the caller-supplied @p data buffer without taking ownership.
            The buffer must cover at least @p width x @p height pixels in
            %Rgb32 format and must remain valid for the lifetime of this
            image. The @p format parameter selects the pixel format and
            defaults to %Rgb32::get().
        */
        Rgb32Image(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                   const Rgb32& format = Rgb32::get());

        /** @brief Constructs an image over external pixel data with row padding.

            Wraps the caller-supplied @p data buffer without taking ownership.
            Each row occupies @p width pixels plus @p padding extra bytes. The
            buffer must be sized accordingly and must remain valid for the
            lifetime of this image. The @p format parameter selects the pixel
            format and defaults to %Rgb32::get().
        */
        Rgb32Image(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                   Pt::ssize_t padding, const Rgb32& format = Rgb32::get());

        /** @brief Copy constructor.

            Creates a deep copy of @p image, including all pixel data.
        */
        Rgb32Image(const Rgb32Image& image);
};

} // namespace Gfx

} // namespace Pt

#endif
