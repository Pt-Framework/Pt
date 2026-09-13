/*
  Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  Copyright (C) 2016 Marc Boris Duerner

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

#ifndef Pt_Forms_Style_h
#define Pt_Forms_Style_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Alignment.h>
#include <Pt/Forms/Direction.h>
#include <Pt/Forms/Spacing.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/TypeInfo.h>
#include <Pt/NonCopyable.h>
#include <Pt/SmartPtr.h>

#include <map>
#include <cstddef>

namespace Pt {

namespace Forms {

class PaintContext;
class Painter;
class Pixmap;
class TextBlock;
class PushButton;
class Menu;
class MenuItem;
class MenuBar;
class MenuBarItem;
class ProgressBar;

/** @brief Manages a reference-counted %Style::Facet.

    Construction, copy, and %reset() take a reference to an existing
    facet. Destruction releases it and destroys the facet when the
    count reaches 0. Pass a facet with a reference count of 0.

    @ingroup Pt-Forms-Styling
*/
template <typename T>
class FacetPtr
{
    public:
        /** @brief Holds @a facet, or 0.
        */
        FacetPtr(T* facet = 0)
        : _facet(facet)
        {
            if( _facet )
                _facet->ref();
        }

        /** @brief Shares the facet held by @a ptr.
        */
        FacetPtr(const FacetPtr& ptr)
        : _facet(ptr._facet)
        {
            if( _facet )
                _facet->ref();
        }

        /** @brief Destructor.
        */
        ~FacetPtr()
        {
            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }
        }

        /** @brief Replaces the held facet with the facet from @a ptr.
        */
        FacetPtr& operator=(const FacetPtr& ptr)
        {
            if(this == &ptr)
                return *this;

            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }

            _facet = ptr._facet;
            if( _facet )
                _facet->ref();

            return *this;
        }

        /** @brief Replaces the held facet with @a facet, or 0.
        */
        void reset(T* facet = 0)
        {
            if (_facet == facet)
                return;

            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }

            _facet = facet;
            if( _facet )
                _facet->ref();
        }

        /** @brief Returns the facet.
        */
        T* operator->() const
        { return _facet; }

        /** @brief Returns the facet.
        */
        T& operator*() const
        { return *_facet; }

        /** @brief Returns true if no facet is held.
        */
        bool operator! () const
        { return _facet == 0; }

        /** @brief Returns true if a facet is held.
        */
        operator bool () const
        { return _facet != 0; }

        /** @brief Returns the facet, or 0.
        */
        T* get()
        { return _facet; }

        /** @brief Returns the facet, or 0.
        */
        const T* get() const
        { return _facet; }

    private:
        T* _facet;
};

/** @brief Stores renderer facets for the active theme.

    A %Style stores one renderer facet per dynamic type. Use %set() to
    register or replace a facet. Use %get() to retrieve it. Use %reset()
    to apply %StyleOptions to every registered facet. %generation()
    changes when the set of facets changes.

    Facets are reference counted. %set() takes a reference. Pass a facet
    with a reference count of 0. Use %Renderer::create() to allocate such
    an instance.

    Use %Application::setStyle() to install a complete style. The
    application starts with %PlatinumStyle. Derive %Renderer, or another
    %Facet, to add drawing for a control family.

    @ingroup Pt-Forms-Styling
*/
class PT_FORMS_API Style
{
    public:
        /** @brief Represents a type-tagged, reference-counted object stored by %Style.

            %typeId() is the registry key. %ref() and %unref() manage the
            reference count. Use %reset() to apply %StyleOptions.
        */
        class Facet : private NonCopyable
        {
            public:
                /** @brief Constructor.
                */
                explicit Facet(const std::type_info& ti, std::size_t refs = 0)
                : _typeId(&ti)
                , _refs(refs)
                {}

                /** @brief Destructor.
                */
                virtual ~Facet()
                {}

                /** @brief Returns the dynamic type used as the registry key.
                */
                const std::type_info& typeId() const
                {
                    return *_typeId;
                }

                /** @brief Adds one reference.
                */
                void ref()
                {
                    ++_refs;
                }

                /** @brief Removes one reference and returns the remaining count.
                */
                std::size_t unref()
                {
                    return --_refs;
                }

                /** @brief Resets this facet with @a options.
                */
                void reset(const StyleOptions& options)
                {
                    onReset(options);
                }

            protected:
                /** @brief Resets this facet with @a options.
                */
                virtual void onReset(const StyleOptions& /*options*/)
                {
                }

            private:
                const std::type_info* _typeId;
                std::size_t _refs;
        };

    public:
        /** @brief Constructor.
        */
        Style();

        /** @brief Copies @a style.
        */
        Style(const Style& style);

        /** @brief Destructor.
        */
        virtual ~Style();

        /** @brief Replaces the contents with a copy of @a style.
        */
        Style& operator=(const Style& style);

        /** @brief Replaces all facets with those from @a style.
        */
        void assign(const Style& style);

        /** @brief Adds or replaces a facet.

            Takes a reference to @a facet. Pass a facet with a reference
            count of 0.
        */
        void set(Facet* facet);

        /** @brief Resets all registered facets with @a options.
        */
        void reset(const StyleOptions& options);

        /** @brief Returns the current style generation.

            The generation changes when the set of facets changes.
        */
        std::size_t generation() const
        {
            return _generation;
        }

        /** @brief Returns the facet of type FacetT, or 0.
        */
        template <typename FacetT>
        FacetT* get() const
        {
            Facet* facet = find( typeid(FacetT) );
            return static_cast<FacetT*>(facet);
        }

    private:
        Facet* find(const std::type_info& ti) const;

    private:
        typedef std::map<TypeInfo, Facet*> FacetMap;
        FacetMap _facets;
        std::size_t _generation;
};

} // namespace

} // namespace

#endif
