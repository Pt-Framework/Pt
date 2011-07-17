/*
 * Copyright (C) 2008 by Marc Boris Duerner
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

class IDecomposer
{
    public:
        virtual ~IDecomposer()
        {}

        virtual void format(Formatter& formatter) = 0;

        virtual void beginFormat(Formatter& formatter) {}

        virtual IDecomposer* advanceFormat(Formatter& formatter) { return 0; }

    protected:
        IDecomposer()
        {}
};


template <typename T>
class Decomposer : public IDecomposer
{
    public:
        Decomposer(SerializationContext* context = 0)
        : _parent(0)
        , _type(0)
        , _si(context)
        , _current(0)
        { }

        void setParent(IDecomposer* parent)
        { _parent = parent; }

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

            if( _si.context() && _si.context()->referencingEnabled() )
            {
                _si.setContextual();
                _si << Pt::save() <<= type;

                _si.clear();
                _si.setName(name);
            }
        }

        virtual void format(Formatter& formatter)
        {
            _si << Pt::save() <<= *_type;
            _si.format(formatter);
        }

        virtual void beginFormat(Formatter& formatter)
        {
            _si << Pt::save() <<= *_type;
            _current = &_si;
            _current->beginFormat(formatter);
            _it = _current->begin();
        }

        virtual IDecomposer* advanceFormat(Formatter& formatter)
        {
            if( _it == _current->end() )
            {
                _current->endFormat(formatter);
                _current = _current->parent();
                if(_current)
                    _it = _current->end();

                return _current != 0 ? this : _parent;
            }

            if( _it->beginFormat(formatter) )
            {
                _it = _current->begin();
                if( _it != _current->end() )
                {
                     _current = &(*_it);
                     return this;
                }
            }

            _it->endFormat(formatter);
            ++_it;
            return this;
        }

    private:
        IDecomposer* _parent;
        const T* _type;
        SerializationInfo _si;
        SerializationInfo* _current;
        SerializationInfo::Iterator _it;
};

} // namespace Pt

#endif
