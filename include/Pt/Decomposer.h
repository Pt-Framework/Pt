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
        //! @brief Destructor
        virtual ~Decomposer()
        {}

        //! @brief Sets the parent
        void setParent(Decomposer* parent)
        { _parent = parent; }

        //! @brief Returns the parent
        Decomposer* parent() const
        { return _parent; }

        /** @brief Format the type completely
        */
        void format(Formatter& formatter)
        { onFormat(formatter); }

        /** @brief Begin formatting the type
        */
        void beginFormat(Formatter& formatter)
        { onBeginFormat(formatter); }

        /** @brief Advance formatting the type
        */
        Decomposer* advanceFormat(Formatter& formatter)
        { return onAdvanceFormat(formatter); }

    protected:
        /** @brief Default constructor.
        */
        Decomposer()
        : _parent(0)
        {}

        /** @brief Format the type completely
        */
        virtual void onFormat(Formatter& formatter)
        {
            onBeginFormat(formatter);

            while( onAdvanceFormat(formatter) != _parent )
                ;
        }

        /** @brief Begin formatting the type
        */
        virtual void onBeginFormat(Formatter& formatter) = 0;

        /** @brief Advance formatting the type
        */
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
        /** @brief Construct with context.
        */
        BasicDecomposer(SerializationContext* context = 0)
        : _type(0)
        , _si(context)
        , _current(0)
        { }

        // TODO: pass instance name to format()/onFormat() and 
        //                             beginFormat()/onBeginFormat()

        /** @brief Begin decomposing a type.
        */
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

    protected:
        // inherit docs
        void onFormat(Formatter& formatter)
        {
            _si << Pt::save() <<= *_type;
            _si.format(formatter);
        }

        // inherit docs
        void onBeginFormat(Formatter& formatter)
        {
            _si << Pt::save() <<= *_type;
            _current = &_si;
            
            _it = _si.beginFormat(formatter);
        }

        // inherit docs
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

                if(_current != 0 )
                    return this;

                _si.clear();
                _type = 0;
                return parent();
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
