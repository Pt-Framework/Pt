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

/** @brief Styler base class.

    Binds a specific renderer or the default renderer to style options.
*/
class PT_FORMS_API StylerBase : private NonCopyable
{
    public:
        /** @brief Constructs an unbound styler.
        */
        StylerBase();

        /** @brief Destructor.
        */
        virtual ~StylerBase();

        /** @brief Binds or rebinds to a style and global style options.
        */
        Renderer* bind(const Style& style, const StyleOptions& styleOptions);

        /** @brief Returns true if bound.
        */
        bool isBound() const;

    protected:
        /** @brief Initializes with a specific renderer.
        */
        void init(Renderer* renderer);

        /** @brief Returns the specific style options.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global) = 0;

        /** @brief Resolves the shared style renderer for the current style.
        */
        virtual Renderer* onStyleRenderer(const Style& style) = 0;

        /** @brief Creates an independant style renderer.
        */
        virtual Renderer* onCreateRenderer(const Style& style) = 0;

    private:
        bool isStyleChanged(const Style& style,
                            const StyleOptions& localOptions) const;

        bool isOptionsChanged(const StyleOptions& options,
                              const StyleOptions& localOptions) const;

    private:
        FacetPtr<Renderer> _renderer;
        std::size_t        _styleGeneration;
        std::size_t        _optionsGeneration;
        std::size_t        _localOptionsGeneration;
        bool               _isRenderer;
        bool               _isOverride;
};

} // namespace

} // namespace

#endif
