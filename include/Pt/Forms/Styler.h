/*
  Copyright (C) 2015 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef Pt_Forms_Styler_h
#define Pt_Forms_Styler_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Renderer.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Forms {

/** @brief Manages renderer binding for one control family.

    A %Styler connects a control family to the current %Style and
    %StyleOptions. Widgets own a derived styler. Applications do not
    construct %Styler. Derive from it only to add a styler for a new
    control family.

    Use %bind() to associate the styler with a style and the
    application options. It returns true when the effective renderer or
    the effective options changed. Call %bind() from %onInvalidate()
    after the base implementation.

    When the overlay contains no local options, %bind() uses the shared
    renderer from the style. When the overlay contains local options, it
    uses a private clone. A renderer assigned through the derived
    %setRenderer() remains until it is cleared. Passing a null renderer
    falls back to the current style on the next %bind().

    %Styler does not provide a public renderer accessor. Measure,
    layout, and paint use typed methods on the derived styler.

    @ingroup Pt-Forms-Styling
*/
class PT_FORMS_API Styler : private NonCopyable
{
    public:
        /** @brief Constructor.
        */
        Styler();

        /** @brief Destructor.
        */
        virtual ~Styler();

        /** @brief Binds this styler to @a style and @a styleOptions.

            Returns true when the effective renderer or the effective
            options changed.
        */
        bool bind(const Style& style, const StyleOptions& styleOptions);

        /** @brief Returns true if a renderer is bound.
        */
        bool isBound() const;

    protected:
        /** @brief Replaces the bound renderer with @a renderer.

            A non-null @a renderer is a custom renderer. %bind() keeps it
            until %init() is called with 0, which falls back to the
            current style.
        */
        void init(Renderer* renderer);

        /** @brief Binds the overlay to @a global and returns it.

            Bind the derived overlay with %StyleOptions::bind() against
            @a global. Do not reset the shared style renderer here.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global) = 0;

        /** @brief Returns the shared style renderer, or 0.

            Do not reset or clone the returned renderer.
        */
        virtual Renderer* onStyleRenderer(const Style& style) = 0;

        /** @brief Creates an independent clone of the style renderer, or 0.

            The clone must have a reference count of 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style) = 0;

    private:
        bool isStyleChanged(const Style& style,
                    const StyleOptions& styleOptions,
                    const StyleOptions& localOptions) const;

        bool isOptionsChanged(const StyleOptions& localOptions) const;

    private:
        FacetPtr<Renderer> _renderer;
        std::size_t        _styleGeneration;
        std::size_t        _optionsGeneration;
        bool               _isRenderer;
        bool               _isDefaultOptions;
};

} // namespace

} // namespace

#endif
