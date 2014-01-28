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
#include <Pt/Types.h>
#include <vector>

namespace Pt {

/** @brief Serializes a set of types.

    @ingroup Serialization
*/
class PT_API Serializer
{
    public:
        /** @brief Default constructor.
        */
        Serializer();

        /** @brief Destructor.
        */
        virtual ~Serializer();

        /** @brief Returns the used context.
        */
        SerializationContext* context();

        /** @brief Clears the serializer and sets a new context.
        */
        void reset(SerializationContext* context);

        /** @brief Returns the used formatter.
        */
        Formatter* formatter();

        /** @brief Returns formatter to use.
        */
        void setFormatter(Formatter& formatter);

        /** @brief Clears all content.
        */
        void clear();

        /** @brief Begins serialization of an object

            This method has to be called for each object to be part of the
            object stream before formatting is done by calling advance() or
            finish(). This is neccessary, because reference IDs have to be
            assigned to referenced objects before they can formatted. The
            string \a name will be used as the instance name of \a type. The
            type must be serializable.
        */
        template <typename T>
        void begin(const T& type, const char* name)
        {
            void* m = this->allocate( sizeof(BasicDecomposer<T>) );
            
            BasicDecomposer<T>* dec = 0;
            try
            {
                dec = new (m) BasicDecomposer<T>(_context);
                dec->begin(type, name);
                _stack.push_back(dec);
            }
            catch(...)
            {
                if(dec)
                    dec->~BasicDecomposer<T>();

                this->deallocate(m);
                throw;
            }
        }

        /** @brief Advances formatting of the object set.

            Returns true if the objects passed to begin() were completely
            formatted, otherwise false is returned, in which case only a
            part of the objects was formatted and advance() has to be called
            again until formatting is complete.
        */
        bool advance();

        /** @brief Finishes parsing of the object set.

            This method will finish formatting of all objects started by
            begin() or partially formatted by calling advance().

        */
        void finish();

    private:
        //! @internal
        void* allocate(std::size_t n);

        //! @internal
        void deallocate(void* p);

    private:
        SerializationContext* _context;
        Formatter* _formatter;
        std::vector<Decomposer*> _stack;
        Decomposer* _current;
        Pt::varint_t _r0; // allocator
};

} // namespace Pt

#endif
