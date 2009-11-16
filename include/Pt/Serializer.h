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
#ifndef Pt_Serializer_h
#define Pt_Serializer_h

#include <Pt/Api.h>
#include <Pt/Formatter.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>

namespace Pt {

class SerializationContext;

class IDecomposer
{
    public:
        virtual ~IDecomposer()
        {}

        virtual void setName(const std::string& name) = 0;

        virtual void format(Formatter& formatter) = 0;

        virtual void beginFormat(Formatter& formatter) {}

        virtual IDecomposer* advance(Formatter& formatter) { return 0; }

    protected:
        IDecomposer()
        {}
};


template <typename T>
class Decomposer : public IDecomposer
{
    public:
        Decomposer()
        : _type(0)
        { }

        void begin(const T& type, SerializationContext* context = 0)
        {
            //std::cerr << "begin " << &type << std::endl;
            _type = &type;

            if(context)
            {
                //*context <<= Pt::sym("name") <<= type;

                Pt::BreakDown b(*context);
                symbolize(b, type, "name");
            }

            _si.clear();
            _si.setContext(context);
            _current = 0;

            //_it = SerializationInfo::Iterator(&_current);
        }

        virtual void setName(const std::string& name)
        {
            // TODO should happen before saving otherwise instance name is not available
            _si.setName(name);
        }

        virtual void format(Formatter& formatter)
        {
            //std::cerr << "format " << _type << std::endl;
            _si <<= Pt::save() <<= *_type;
            _si.format(formatter);
        }

        void beginFormat(Formatter& formatter)
        {
            _current = &_si;
            _current->beginFormat(formatter);
            _it = _current->begin();
        }

        IDecomposer* advance(Formatter& formatter)
        {
            if( _it == _current->end() )
            {
                _current->endFormat(formatter);
                _current = _current->parent();
                if(_current)
                    _it = _current->end();

                return _current != 0 ? this : 0;
            }

            if( _it->beginFormat(formatter) )
            {
                // _it = _current->begin();
                // if( _it != _current->end() )
                // {
                //     _current = &(*_it);
                //     return true;
                // }
            }

            _it->endFormat(formatter);
            ++_it;
            return this;
        }

    private:
        const T* _type;
        SerializationInfo _si;
        SerializationInfo* _current;
        SerializationInfo::Iterator _it; // TODO iterator stack !!!
};

class SerializerBase : public Formatter
{
    public:
        SerializerBase()
        {}

        virtual ~SerializerBase()
        {}

        SerializationContext& context()
        { return *_context; }

        const SerializationContext& context() const
        { return *_context; }

        void setContext(SerializationContext& context)
        { _context = &context; }

        /** @brief Serialize an object

            The serializer will serialize the object \a type. The string
            \a name will be used as the instance name of \a type. The
            type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            Decomposer<T>* dec = new Decomposer<T>;
            _heap.push_back(dec);
            _stack.push_back(dec);

            dec->begin(type, _context);
            dec->setName(name);
            this->begin(dec);
        }

        void finish()
        {
            std::vector<IDecomposer*>::iterator it;

            for(it = _stack.begin(); it != _stack.end(); ++it)
            {
                (*it)->format(*this);
            }

            for(it = _heap.begin(); it != _heap.end(); ++it)
            {
                delete *it;
            }

            _heap.clear();
            _stack.clear();
        }

    protected:
        virtual void begin(IDecomposer& dec) = 0;

    private:
        SerializationContext* _context;
        std::vector<IDecomposer*> _stack;
        std::vector<IDecomposer*> _heap;
};

} // namespace Pt

#endif
