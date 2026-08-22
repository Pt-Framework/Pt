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
#include <Pt/Forms/StyleOptions.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Forms {

/** @brief Renderer-binding controller for extracted style slices.

    Owns the currently bound %Renderer and the custom renderer source.
    Detects when a derived styler must acquire a new renderer or refresh
    its tokens. Derived classes resolve a typed renderer in
    %onStyleRenderer and apply tokens in %onBindOptions. Shared style
    renderers are never prepared here; they are synchronized through
    %Style::reset().
*/
class PT_FORMS_API StylerBase : private NonCopyable
{
    public:
        /** @brief Constructs an unbound style controller.
        */
        StylerBase();

        /** @brief Binds or refreshes the renderer for the current style.

            Local options come from %onLocalOptions. Uses the custom renderer
            when one is assigned, otherwise calls %onStyleRenderer and clones
            the result through %onCreateRenderer if local overrides exist.
            Calls %onBindOptions when the bound renderer is custom or an
            override clone and the global or local options generation changed.
            Unchanged bindings are left in place.
        */
        Renderer* bind(const Style& style, const StyleOptions& options);

        /** @brief Returns true if a renderer is currently bound.
        */
        bool isBound() const;

    protected:
        /** @brief Assigns an externally owned custom renderer.

            Stores the custom source and marks it dirty so the next %bind
            call reacquires it. A null renderer falls back to the current
            style. Pointer identity is not compared; addresses can be reused.
        */
        void apply(Renderer* renderer);

        /** @brief Returns the widget-local style options.

            Options always exist. The returned object is used for override
            detection and option-generation checks.
        */
        virtual const StyleOptions& onLocalOptions() const = 0;

        /** @brief Resolves the renderer for the current style source.

            Must return the shared style renderer to bind, or 0 if none is
            available. Must not prepare the returned renderer. The returned
            renderer is cloned automatically by %onCreateRenderer when local
            overrides exist. Custom renderers are handled by %StylerBase.
        */
        virtual Renderer* onStyleRenderer(const Style& style) = 0;

        /** @brief Creates an independant renderer.

            Called by %bind when the renderer returned by %onStyleRenderer
            is a shared style prototype and local overrides exist. The
            implementation must allocate and return a new renderer of the
            same concrete type. Must not prepare the returned renderer.
        */
        virtual Renderer* onCreateRenderer(const Style& style) = 0;

        /** @brief Applies tokens to the currently bound renderer.

            Called only after a renderer has been bound and only for custom
            renderers and override clones. Shared style prototypes stay
            synchronized through %Style::reset().
        */
        virtual void onBindOptions(const StyleOptions& options) = 0;

    private:
        /** @brief Returns true if the style source must be rebound.
        */
        bool isStyleChanged(const Style& style,
                            const StyleOptions& localOptions) const;

        /** @brief Returns true if the bound renderer must refresh its tokens.
        */
        bool isOptionsChanged(const StyleOptions& options,
                              const StyleOptions& localOptions) const;

    private:
        static const std::size_t InvalidGeneration;

        FacetPtr<Renderer> _renderer;
        FacetPtr<Renderer> _custom;
        std::size_t        _styleGeneration;
        std::size_t        _optionsGeneration;
        std::size_t        _localOptionsGeneration;
        bool               _hasOverrides;
        bool               _customChanged;
};

} // namespace

} // namespace

#endif
