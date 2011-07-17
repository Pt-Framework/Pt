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
#ifndef Pt_Deserializer_h
#define Pt_Deserializer_h

#include <Pt/Api.h>
#include <Pt/Composer.h>
#include <Pt/Formatter.h>
#include <Pt/SerializationContext.h>

namespace Pt {

class PT_API Deserializer
{
    public:
        Deserializer();

        virtual ~Deserializer();

        SerializationContext* context();

        void reset(SerializationContext* context);

        Formatter* formatter();

        void setFormatter(Formatter& formatter);

        void clear();

        /** @brief Deserialize an object

            This method will deserialize the object \a type.
            The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            Composer<T> comp(_context);
            comp.begin(type);
            _fmt->parse(comp);
        }

        template <typename T>
        void begin(T& type)
        {
            void* m = this->allocate( sizeof(Composer<T>) );
            Composer<T>* composer = new (m) Composer<T>(_context);
            _current = composer;
            composer->begin(type);
        }

        bool advance();

        void finish();

    private:
        void* allocate(size_t n);

        void deallocate(void* p);

    private:
        SerializationContext* _context;
        Formatter* _fmt;
        IComposer* _current;
        void* _mem;
        std::size_t _memsize;
};

} // namespace Pt

#endif
