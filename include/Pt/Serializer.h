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

class PT_API Serializer
{
    public:
        Serializer();

        virtual ~Serializer();

        SerializationContext* context();

        void reset(SerializationContext* context);

        Formatter* formatter();

        void setFormatter(Formatter& formatter);

        void clear();

        /** @brief Serialize an object

            The serializer will serialize the object \a type. The string
            \a name will be used as the instance name of \a type. The
            type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const char* name)
        {
            void* m = this->allocate( sizeof(Decomposer<T>) );
            Decomposer<T>* dec = new (m) Decomposer<T>(_context);
            _stack.push_back(dec);

            dec->begin(type, name);
        }

        void begin();

        bool advance();

        void finish();

    private:
        void* allocate(size_t n);

        void deallocate(void* p);

    private:
        SerializationContext* _context;
        Formatter* _formatter;
        std::vector<IDecomposer*> _stack;
        IDecomposer* _current;
        void* _alloc;
};

} // namespace Pt

#endif
