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

#ifndef Pt_Forms_StylerBase_h
#define Pt_Forms_StylerBase_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Renderer.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Forms {

/** @brief Renderer-binding controller for extracted style slices.

    Owns the currently bound %Renderer and detects when a derived styler
    must acquire a new renderer or refresh its tokens. Derived classes
    resolve a typed renderer in %onBindStyle and apply tokens in
    %onBindOptions. Shared style renderers are never prepared here; they
    are synchronized through %Style::reset().
*/
class PT_FORMS_API StylerBase : private NonCopyable
{
    public:
        /** @brief Constructs an unbound style controller.
        */
        StylerBase();

        /** @brief Binds or refreshes the renderer for the current style.

            Calls %onBindStyle when the style source changed and
            %onBindOptions when the global or derived options changed.
            Unchanged bindings are left in place.
        */
        Renderer* bind(const Style& style, const StyleOptions& options);

        /** @brief Returns true if a renderer is currently bound.
        */
        bool isBound() const;

    protected:
        /** @brief Returns true if the style source must be rebound.
        */
        bool isStyleChanged(const Style& style) const;

        /** @brief Returns true if the bound renderer must refresh its tokens.
        */
        bool isOptionsChanged(const StyleOptions& options) const;

        /** @brief Hook for derived style-source changes.

            The default implementation returns false.
        */
        virtual bool onIsStyleChanged() const;

        /** @brief Hook for derived option changes.

            The default implementation returns false.
        */
        virtual bool onIsOptionsChanged() const;

        /** @brief Resolves the renderer for the current style source.

            Must return the renderer to bind, or 0 if none is available.
            Must not prepare the returned renderer.
        */
        virtual Renderer* onBindStyle(const Style& style) = 0;

        /** @brief Applies tokens to the currently bound renderer.

            Called only after a renderer has been bound. Shared style
            prototypes should be left untouched by the derived styler.
        */
        virtual void onBindOptions(const StyleOptions& options) = 0;

    private:
        static const std::size_t InvalidGeneration;

        FacetPtr<Renderer> _renderer;
        std::size_t        _styleGeneration;
        std::size_t        _optionsGeneration;
};

} // namespace

} // namespace

#endif
