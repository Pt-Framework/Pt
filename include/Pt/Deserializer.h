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
#include <Pt/NonCopyable.h>
#include <Pt/SerializationContext.h>

namespace Pt {

/** @brief Deserializes a set of types.

    @ingroup Serialization
*/
class PT_API Deserializer : private NonCopyable
{
    public:
        Deserializer();

        virtual ~Deserializer();

        SerializationContext* context();

        void reset(SerializationContext* context);

        Formatter* formatter();

        void setFormatter(Formatter& formatter);

        void clear();

        /** @brief Starts parsing of an object.

            This method will begin to parse the object \a t. The type \a t must
            be serializable. Parsing can be completed by calling advance()
            repeatedly, or by calling finish().
        */
        template <typename T>
        void begin(T& t)
        {
            if( ! _fmt)
                return;

            // allocate() also destructs _current
            void* m = this->allocate( sizeof(BasicComposer<T>) );

            BasicComposer<T>* comp = 0;
            try
            {
                comp = new (m) BasicComposer<T>(_context);
                comp->begin(t);
                _current = comp;
            }
            catch(...)
            {
                if(comp)
                    comp->~BasicComposer<T>();

                this->deallocate(m);
                throw;
            }

            _fmt->beginParse(*_current);
        }

        /** @brief Advances parsing of an object.

            Returns true if the object passed to begin() could be parsed
            completely, otherwise false is returned. If false is returned,
            no further progress can be made, until more data becomes available.
            When parsing is complete, references can be fixed up by calling
            fixup().
        */
        bool advance();

        /** @brief Finishes parsing of an object.

            This method will finish parsing of the object started by begin().
            After all objects have been parsed, references can be fixed up by
            calling fixup()
        */
        void finish();

        /** @brief Fixes up references.

            Weak references between the parsed objects can be fixed up calling
            this method. Fixup has to happen after all objects have been parsed
            so references can be forward and backward in the object stream.
        */
        void fixup();

    private:
        //! @internal
        void* allocate(std::size_t n);

        //! @internal
        void deallocate(void* p);

    private:
        SerializationContext* _context;
        Formatter* _fmt;
        Composer* _current;
        void* _mem;
        std::size_t _memsize;
        Pt::varint_t _r0; // allocator
};

} // namespace Pt

#endif
