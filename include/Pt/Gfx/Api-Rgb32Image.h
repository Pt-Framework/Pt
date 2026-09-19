#ifndef PT_GFX_API_RGB32IMAGE_H
#define PT_GFX_API_RGB32IMAGE_H

#include <Pt/Gfx/Api.h>

namespace Pt {

namespace Gfx {

/** @brief Premultiplied ARGB-32 image.

    %Rgb32Image is %BasicImage with the %Rgb32 format bound at
    compile time. Each pixel is 32 bits with 8-bit channels in
    premultiplied ARGB-32 layout: when a color is written, the RGB
    components are multiplied by alpha. Use this type when blending
    should not divide by alpha per pixel. Use %Argb32Image when
    channels are stored with straight alpha.

    Constructors that take width and height allocate. Constructors
    that take a data pointer wrap the caller buffer; that buffer must
    remain valid for the lifetime of the image.

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
