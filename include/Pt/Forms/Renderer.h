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

#ifndef Pt_Forms_Renderer_h
#define Pt_Forms_Renderer_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

/** @brief Cloneable style facet used by widget renderers.

    Extends %Style::Facet with a prototype clone operation. %create()
    returns a freshly allocated instance with a reference count of 0.
    Shared theme instances stay in %Style and are synchronized through
    %reset(), not by cloning.
*/
class PT_FORMS_API Renderer : public Style::Facet
{
    public:
        /** @brief Constructs a renderer facet of the given dynamic type.
        */
        explicit Renderer(const std::type_info& ti, std::size_t refs = 0)
        : Facet(ti, refs)
        {
        }

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        Renderer* create() const
        {
            return onCreate();
        }

        virtual void prepare(const StyleOptions& options) = 0;

    protected:
        /** @brief Hook that allocates a new renderer of the same concrete type.
        */
        virtual Renderer* onCreate() const = 0;
};

} // namespace

} // namespace

#endif
