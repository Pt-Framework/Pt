/*
 * Copyright (C) 2008-2013 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_Decomposer_h
#define Pt_Decomposer_h

#include <Pt/Api.h>
#include <Pt/Formatter.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>

namespace Pt {

/** @brief Manages the decomposition of types during serialization.

    @ingroup Serialization
*/
class Decomposer
{
    public:
        virtual ~Decomposer()
        {}

        void setParent(Decomposer* parent)
        { _parent = parent; }

        Decomposer* parent() const
        { return _parent; }

        void format(Formatter& formatter)
        { onFormat(formatter); }

        void beginFormat(Formatter& formatter)
        { onBeginFormat(formatter); }

        Decomposer* advanceFormat(Formatter& formatter)
        { return onAdvanceFormat(formatter); }

    protected:
        Decomposer()
        : _parent(0)
        {}

        virtual void onFormat(Formatter& formatter)
        {
            onBeginFormat(formatter);

            while( onAdvanceFormat(formatter) != _parent )
                ;
        }

        virtual void onBeginFormat(Formatter& formatter) = 0;

        virtual Decomposer* onAdvanceFormat(Formatter& formatter) = 0;

    private:
        Decomposer* _parent;
};

/** @brief Manages the decomposition of types during serialization.

    @ingroup Serialization
*/
template <typename T>
class BasicDecomposer : public Decomposer
{
    public:
        BasicDecomposer(SerializationContext* context = 0)
        : _type(0)
        , _si(context)
        , _current(0)
        { }

        void begin(const T& type, const char* name)
        {
            if(_type)
            {
                _si.clear();
                _it = SerializationInfo::Iterator();
                _current = 0;
            }

            _type = &type;
            _si.setName(name);

            Pt::SerializationContext* ctx = _si.context();
            if( ctx && ctx->isReferencing() )
            {
                *ctx << Pt::save() <<= type;
            }
        }

        void onFormat(Formatter& formatter)
        {
            _si << Pt::save() <<= *_type;
            _si.format(formatter);
        }

        void onBeginFormat(Formatter& formatter)
        {
            _si << Pt::save() <<= *_type;
            _current = &_si;
            
            _it = _si.beginFormat(formatter);
        }

        Decomposer* onAdvanceFormat(Formatter& formatter)
        {
            if( _it == _current->end() )
            {
                _current->endFormat(formatter);
                
                if( _current->sibling() )
                {
                    _current = _current->sibling();
                    _it = _current->beginFormat(formatter);
                }
                else
                {
                    _current = _current->parent();
                    if(_current)
                        _it = _current->end();
                }

                return _current != 0 ? this : parent();
            }

            SerializationInfo::Iterator it = _it->beginFormat(formatter);
            if( it != _it->end() )
            {
                 _current = &(*_it);
                 _it = it;
            }
            else
            {
                _it->endFormat(formatter);
                ++_it;
            }
            
            return this;
        }

    private:
        const T* _type;
        SerializationInfo _si;
        SerializationInfo* _current;
        SerializationInfo::Iterator _it;
};

} // namespace Pt

#endif
