/* Copyright (C) 2019 Marc Boris Duerner

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

#ifndef Pt_Forms_Icon_h
#define Pt_Forms_Icon_h

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Image.h>
#include <Pt/System/Path.h>

#include <vector>

namespace Pt {

namespace Forms {

class IconImpl;

/** @brief Provides images for %Icon objects.

    Derive from %IconProvider only to provide a custom image source. Ordinary
    code uses the built-in provider created by a default %Icon. A custom
    provider implements every pure virtual function. Its %getImage() function
    selects and, when needed, loads an image for the requested size.

    An %Icon constructed with a provider does not own it. Keep the provider
    alive while an icon uses it. Destroying a provider detaches its remaining
    icons. Calling %Icon::getImage() on a detached icon is invalid.

    @ingroup Pt-Forms-Icons
*/
class PT_FORMS_API IconProvider
{
    friend class IconImpl;

    public:
        /** @brief Creates an unused provider.
        */
        IconProvider();

        /** @brief Detaches remaining icons so they no longer use this provider.
        */
        virtual ~IconProvider();

        /** @brief Returns true when no images are registered.
        */
        virtual bool empty() const = 0;

        /** @brief Removes all registered images.
        */
        virtual void clear() = 0;

        /** @brief Registers @a image at @a size.
        */
        virtual void addImage(const Gfx::SizeF& size, const Gfx::Image& image) = 0;

        /** @brief Registers a file path at @a size.

            The image loads on first use.
        */
        virtual void addImage(const Gfx::SizeF& size, const System::Path& path) = 0;

        /** @brief Returns the smallest registered size, or an empty size.
        */
        virtual Gfx::SizeF minimumSize() const = 0;

        /** @brief Returns the largest registered size, or an empty size.
        */
        virtual Gfx::SizeF maximumSize() const = 0;

        /** @brief Returns an image for the requested @a area.
        */
        virtual const Gfx::Image& getImage(const Gfx::SizeF& area) = 0;

    private:
        void attachIcon(IconImpl* parent);

        void detachIcon(IconImpl* parent);

    private:
        std::vector<IconImpl*> _icons;
};


/** @brief Represents an image at one or more logical sizes.

    An %Icon is not a %Widget. It supplies images to controls, which request
    an image for a layout area with %getImage(). %Label, %PushButton, %Panel,
    and list items accept an %Icon through %setIcon().

    A default icon uses a built-in provider. %Icon(IconProvider&) attaches a
    caller-owned provider that must outlive the icon. Copies share the same
    implementation until %addImage() or %clear() changes one copy.

    %addImage() registers an image or file path at a logical size. Passing
    only an image uses its pixel size as the key. The logical size may differ
    from the pixel size. Images registered by path load as PNG on first use
    through %Application::loadImage(). A missing path produces an empty image.

    %getImage() returns the registered image for a requested area. The
    built-in provider selects the largest image that fits entirely in that
    area. An empty icon produces an empty image. %minimumSize() and
    %maximumSize() return the smallest and largest registered keys.

    @code
    Pt::Forms::Icon icon;
    icon.addImage(Pt::Gfx::SizeF(16, 16), Pt::System::Path("app-16.png"));
    icon.addImage(Pt::Gfx::SizeF(32, 32), Pt::System::Path("app-32.png"));

    Pt::Forms::Label label;
    label.setIcon(icon, Pt::Gfx::SizeF(16, 16));
    label.setText("Hello");
    @endcode

    @ingroup Pt-Forms-Icons
*/
class PT_FORMS_API Icon
{
    public:
        /** @brief Creates an empty icon with a built-in provider.
        */
        Icon();

        /** @brief Shares images with @a icon until %addImage() or %clear().
        */
        Icon(const Icon& icon);

        /** @brief Attaches @a provider. The icon does not own it.
        */
        Icon(IconProvider& provider);

        /** @brief Releases this copy.

            The last copy destroys the shared images and detaches from a
            custom provider.
        */
        ~Icon();

        /** @brief Shares images with @a icon until %addImage() or %clear().
        */
        Icon& operator=(const Icon& icon);

        /** @brief Returns true when no images are registered.
        */
        bool empty() const;

        /** @brief Removes all images from this icon.

            Copies of this icon are not affected.
        */
        void clear();

        /** @brief Registers @a image using its pixel size as the key.

            Copies of this icon are not affected.
        */
        void addImage(const Gfx::Image& image);

        /** @brief Registers @a image at logical size @a size.

            Copies of this icon are not affected. @a size may differ from
            the image pixel size.
        */
        void addImage(const Gfx::SizeF& size, const Gfx::Image& image);

        /** @brief Registers a file path at logical size @a size.

            The image loads on first use. Copies of this icon are not
            affected.
        */
        void addImage(const Gfx::SizeF& size, const System::Path& path);

        /** @brief Registers a file path at @a width by @a height.
        */
        void addImage(double width, double height, const System::Path& path);

        /** @brief Returns an image for the requested @a area.

            May load a path image on first use.
        */
        const Gfx::Image& getImage(const Gfx::SizeF& area) const;

        /** @brief Returns the smallest registered size, or an empty size.
        */
        Gfx::SizeF minimumSize() const;

        /** @brief Returns the largest registered size, or an empty size.
        */
        Gfx::SizeF maximumSize() const;

    private:
        void detach();

    private:
        mutable class IconImpl* _impl;
};

} // namespace

} // namespace

#endif
