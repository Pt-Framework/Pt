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
#include <Pt/Decomposer.h>
#include <Pt/SerializationContext.h>
#include <vector>

namespace Pt {

class Serializer
{
    public:
        Serializer()
        : _context(0)
        , _formatter(0)
        {}

        virtual ~Serializer()
        {
            std::vector<IDecomposer*>::iterator it;
            for(it = _heap.begin(); it != _heap.end(); ++it)
            {
                delete *it;
            }

            _heap.clear();
            _stack.clear();
        }

        SerializationContext* context()
        { return _context; }

        Formatter& formatter()
        { return *_formatter; }

        void setFormatter(Formatter& formatter)
        { _formatter = &formatter; }

        void clear()
        {
            if(_context)
                _context->reset();

            std::vector<IDecomposer*>::iterator it;
            for(it = _heap.begin(); it != _heap.end(); ++it)
            {
                delete *it;
            }

            _heap.clear();
            _stack.clear();
        }

        void reset(SerializationContext* context)
        {
            if(_context)
                _context->reset();

            std::vector<IDecomposer*>::iterator it;
            for(it = _heap.begin(); it != _heap.end(); ++it)
            {
                delete *it;
            }

            _heap.clear();
            _stack.clear();

            _context = context;
        }

        void queue(IDecomposer& dec)
        {
            dec.setContext(_context);
            _stack.push_back(&dec);
        }

        /** @brief Serialize an object

            The serializer will serialize the object \a type. The string
            \a name will be used as the instance name of \a type. The
            type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const char* name)
        {
            Decomposer<T>* dec = new Decomposer<T>;
            _heap.push_back(dec);
            _stack.push_back(dec);

            dec->setContext(_context);
            dec->begin(type, name);
        }

        void beginFormat()
        {
            _current = 0;
            if( _stack.empty() )
                return;

            _current = _stack.front();
            _current->beginFormat(*_formatter);
        }

        bool advance()
        {
            if( ! _current )
                return false;

            _current = _current->advance(*_formatter);
            if( _current )
                return true;

            // at least one on the stack, otherwise _current is 0
            _stack.front()->clear();
            _stack.erase( _stack.begin() );

            if( _stack.empty() )
                return false;

            _current = _stack.front();
            _current->beginFormat(*_formatter);
            return true;
        }

        void finish()
        {
            std::vector<IDecomposer*>::iterator it;

            for(it = _stack.begin(); it != _stack.end(); ++it)
            {
                (*it)->format(*_formatter);
            }

            for(it = _heap.begin(); it != _heap.end(); ++it)
            {
                delete *it;
            }

            _heap.clear();
            _stack.clear();
        }

    private:
        SerializationContext* _context;
        Formatter* _formatter;
        std::vector<IDecomposer*> _stack;
        std::vector<IDecomposer*> _heap;
        IDecomposer* _current;
};

} // namespace Pt

#endif
