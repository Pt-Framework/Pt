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

template <typename T>
class FacetPtr
{
    public:
        FacetPtr(T* facet = 0)
        : _facet(facet)
        {
            if( _facet )
                _facet->ref();
        }

        FacetPtr(const FacetPtr& ptr)
        : _facet(ptr._facet)
        {
            if( _facet )
                _facet->ref();
        }

        ~FacetPtr()
        {
            if(_facet)
            {
                if( 0 == _facet->unref() )
                    delete _facet;
            }
        }

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

        T* operator->() const
        { return _facet; }

        T& operator*() const
        { return *_facet; }

        bool operator! () const
        { return _facet == 0; }

        operator bool () const
        { return _facet != 0; }

        T* get()
        { return _facet; }

        const T* get() const
        { return _facet; }

    private:
        T* _facet;
};

/** @brief Style for widgets.
 */
class PT_FORMS_API Style
{
    public:
        class Facet : private NonCopyable
        {
            public:
                explicit Facet(const std::type_info& ti, std::size_t refs = 0)
                : _typeId(&ti)
                , _refs(refs)
                {}

                virtual ~Facet()
                {}

                const std::type_info& typeId() const
                {
                    return *_typeId;
                }

                void ref()
                {
                    ++_refs;
                }

                std::size_t unref()
                {
                    return --_refs;
                }

                /** @brief Resets the facet to the given global style options.
                */
                void reset(const StyleOptions& options)
                {
                    onReset(options);
                }

            protected:
                /** @brief Hook for facet-specific global reset logic.
                */
                virtual void onReset(const StyleOptions& /*options*/)
                {
                }

            private:
                const std::type_info* _typeId;
                std::size_t _refs;
        };

    public:
        /** @brief Constructs an empty style.
        */
        Style();

        /** @brief Constructs a style by copying another style.
        */
        Style(const Style& style);

        /** @brief Destroys the style and releases all registered facets.
        */
        virtual ~Style();

        /** @brief Assigns the contents of another style.
        */
        Style& operator=(const Style& style);

        /** @brief Replaces all facets with those from another style.
        */
        void assign(const Style& style);

        /** @brief Registers or replaces a facet.
        */
        void set(Facet* facet);

        /** @brief Resets all registered facets to the given global style options.
        */
        void reset(const StyleOptions& options);

        /** @brief Returns the current style generation.

            The generation changes whenever the style content of this instance
            changes and can be used to detect when a widget must rebind to a
            different renderer source.
        */
        std::size_t generation() const
        {
            return _generation;
        }

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
