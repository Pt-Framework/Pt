/* Copyright (C) 2026 Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 Marc Boris Duerner

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

#ifndef Pt_Forms_Styler_h
#define Pt_Forms_Styler_h

#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

/** @brief Common renderer-binding controller for %Forms style slices.

    Manages the active renderer, binding mode, and generation counters
    needed for local prepare bookkeeping. All %Forms style controllers
    that follow the extracted-slice pattern derive from this template.

    @tparam RendererT     Concrete %Style::Facet renderer type. Must expose
                          %create() and %prepare(const StyleOptions&, const OptionsT&).
    @tparam OptionsT      Widget-local style options type. Must expose
                          %hasOverrides() and %generation() const.
*/
template <typename RendererT,
          typename OptionsT>
class Styler : private NonCopyable
{
    public:
        /** @brief Constructs an unbound style controller.
        */
        Styler();

        /** @brief Returns true if a renderer is currently bound.
        */
        bool isBound() const;

        /** @brief Returns true if a custom renderer is currently bound.
        */
        bool isCustom() const;

        /** @brief Binds to the current style renderer path.

            Selects either the shared style renderer (%SharedRenderer) or a
            private override clone (%CustomOverrides). Shared style renderers
            are synchronized through %Style::reset() and are therefore not
            prepared locally here. Private override clones are prepared
            immediately with the current style and local options. This call
            always leaves custom renderer mode.
        */
        RendererT* bind(const Pt::Forms::Style& style,
                        const StyleOptions& options,
                        const OptionsT& localOptions);

        /** @brief Binds to an externally assigned renderer.

            Assigns a custom renderer explicitly and prepares it with the
            current options.
        */
        RendererT* bind(RendererT& renderer,
                        const StyleOptions& options,
                        const OptionsT& localOptions);

        /** @brief Refreshes the current renderer binding.

            Custom renderers stay assigned and are re-prepared only when the
            palette or local options generation changed. Shared renderers and
            override clones are reacquired through the style-path bind when
            the %Style generation changed or the override mode flipped.
            Override clones with an unchanged source are re-prepared in place.
        */
        RendererT* rebind(const Pt::Forms::Style& style,
                          const StyleOptions& options,
                          const OptionsT& localOptions);

        /** @brief Returns the currently bound renderer or 0 if none is available.
        */
        RendererT* renderer();

        /** @brief Returns the currently bound renderer or 0 if none is available.
        */
        const RendererT* renderer() const;

    protected:
        enum Binding
        {
            SharedRenderer,
            CustomOverrides,
            CustomRenderer
        };

    private:
        FacetPtr<RendererT> _renderer;
        Binding             _binding;
        std::size_t         _boundStyleGeneration;
        std::size_t         _styleOptionsGeneration;
        std::size_t         _localOptionsGeneration;
};


template <typename RendererT,
          typename OptionsT>
Styler<RendererT, OptionsT>::Styler()
: _binding(SharedRenderer)
, _boundStyleGeneration( std::size_t(-1) )
, _styleOptionsGeneration( std::size_t(-1) )
, _localOptionsGeneration( std::size_t(-1) )
{
}


template <typename RendererT,
          typename OptionsT>
bool Styler<RendererT, OptionsT>::isBound() const
{
    return _renderer != 0;
}


template <typename RendererT,
          typename OptionsT>
bool Styler<RendererT, OptionsT>::isCustom() const
{
    return _binding == CustomRenderer;
}


template <typename RendererT,
          typename OptionsT>
RendererT*
Styler<RendererT, OptionsT>::bind(const Pt::Forms::Style& style,
                                  const StyleOptions& options,
                                  const OptionsT& localOptions)
{
    _styleOptionsGeneration = std::size_t(-1);
    _localOptionsGeneration = std::size_t(-1);

    if( localOptions.hasOverrides() )
    {
        RendererT* renderer = style.get<RendererT>();
        if( renderer )
            renderer = renderer->create();

        _renderer.reset(renderer);
        _binding = CustomOverrides;

        if( _renderer )
        {
            _renderer->prepare(options, localOptions);
            _styleOptionsGeneration = options.generation();
            _localOptionsGeneration = localOptions.generation();
        }
    }
    else
    {
        _renderer.reset( style.get<RendererT>() );
        _binding = SharedRenderer;
    }

    _boundStyleGeneration = style.generation();

    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
RendererT*
Styler<RendererT, OptionsT>::bind(RendererT& renderer,
                                  const StyleOptions& options,
                                  const OptionsT& localOptions)
{
    _renderer.reset(&renderer);

    _binding = CustomRenderer;
    _boundStyleGeneration = std::size_t(-1);
    _styleOptionsGeneration = std::size_t(-1);
    _localOptionsGeneration = std::size_t(-1);

    _renderer->prepare(options, localOptions);

    _styleOptionsGeneration = options.generation();
    _localOptionsGeneration = localOptions.generation();

    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
RendererT*
Styler<RendererT, OptionsT>::rebind(const Pt::Forms::Style& style,
                                    const StyleOptions& options,
                                    const OptionsT& localOptions)
{
    const bool optionsChanged = _styleOptionsGeneration != options.generation() ||
                                _localOptionsGeneration != localOptions.generation();

    if( _binding == CustomRenderer )
    {
        if( _renderer && optionsChanged )
            _renderer->prepare(options, localOptions);

        if( _renderer )
        {
            _styleOptionsGeneration = options.generation();
            _localOptionsGeneration = localOptions.generation();
        }

        return _renderer.get();
    }

    const bool unbound = ! _renderer;
    const bool styleChanged = _boundStyleGeneration != style.generation();
    const bool overrideChanged = localOptions.hasOverrides() != (_binding == CustomOverrides);

    if( unbound || styleChanged || overrideChanged )
        return bind(style, options, localOptions);

    if( _binding == CustomOverrides && optionsChanged )
    {
        _renderer->prepare(options, localOptions);
        _styleOptionsGeneration = options.generation();
        _localOptionsGeneration = localOptions.generation();
    }

    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
RendererT* Styler<RendererT, OptionsT>::renderer()
{
    return _renderer.get();
}


template <typename RendererT,
          typename OptionsT>
const RendererT* Styler<RendererT, OptionsT>::renderer() const
{
    return _renderer.get();
}


} // namespace

} // namespace

#endif
