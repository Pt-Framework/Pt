#ifndef PT_GFX_API_ARGB32IMAGE_H
#define PT_GFX_API_ARGB32IMAGE_H

#include <Pt/Gfx/Api.h>

namespace Pt {

namespace Gfx {

/** @brief Image storing pixels in ARGB-32 format.

    %Argb32Image owns its pixel data and stores each pixel as a 32-bit value
    with four 8-bit channels: alpha, red, green, and blue.

    @headerfile Argb32Image.h <Pt/Gfx/Argb32Image.h>
    @ingroup Pt-Gfx-Images
*/
class Argb32Image : public BasicImage<Argb32>
{
    public:
        /** @brief Constructs an empty image.

            Creates an image with no pixel data. The @p format parameter
            selects the pixel format and defaults to %Argb32::get().
        */
        explicit Argb32Image(const Argb32& format = Argb32::get());

        /** @brief Constructs an image of the given size.

            Allocates pixel data for @p width x @p height pixels. The
            @p format parameter selects the pixel format and defaults to
            %Argb32::get().
        */
        Argb32Image(Pt::ssize_t width, Pt::ssize_t height,
                    const Argb32& format = Argb32::get());

        /** @brief Constructs an image of the given size with row padding.

            Allocates pixel data for @p width x @p height pixels. Each row
            is extended by @p padding extra bytes beyond the last pixel,
            which is useful when the image data must be aligned to a specific
            stride. The @p format parameter selects the pixel format and
            defaults to %Argb32::get().
        */
        Argb32Image(Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding,
                    const Argb32& format = Argb32::get());

        /** @brief Constructs an image over external pixel data.

            Wraps the caller-supplied @p data buffer without taking ownership.
            The buffer must cover at least @p width x @p height pixels in
            %Argb32 format and must remain valid for the lifetime of this
            image. The @p format parameter selects the pixel format and
            defaults to %Argb32::get().
        */
        Argb32Image(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                    const Argb32& format = Argb32::get());

        /** @brief Constructs an image over external pixel data with row padding.

            Wraps the caller-supplied @p data buffer without taking ownership.
            Each row occupies @p width pixels plus @p padding extra bytes. The
            buffer must be sized accordingly and must remain valid for the
            lifetime of this image. The @p format parameter selects the pixel
            format and defaults to %Argb32::get().
        */
        Argb32Image(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                    Pt::ssize_t padding, const Argb32& format = Argb32::get());

        /** @brief Copy constructor.

            Creates a deep copy of @p image, including all pixel data.
        */
        Argb32Image(const Argb32Image& image);
};

} // namespace Gfx

} // namespace Pt

#endif
