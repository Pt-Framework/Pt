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
#include <Pt/SerializationContext.h>
#include <map>

namespace Pt {

class Deserializer
{
    public:
        Deserializer()
        : _context(0)
        , _current(0)
        {}

        virtual ~Deserializer()
        {
            delete _current;
            _current = 0;
        }

        SerializationContext* context()
        { return _context; }

        void clear()
        {
            if(_context)
                _context->reset();

            delete _current;
            _current = 0;
        }

        void reset(SerializationContext* context)
        {
            if(_context)
                _context->reset();

            delete _current;
            _current = 0;

            _context = context;
        }

        /** @brief Deserialize an object

            This method will deserialize the object \a type.
            The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            Composer<T> deser;
            deser.setContext(_context);
            deser.begin(type);

            this->get(deser);
        }

        template <typename T>
        void begin(T& type)
        {
            delete _current;
            _current = 0;

            Composer<T>* composer = new Composer<T>;
            _current = composer;

            composer->setContext(_context);
            composer->begin(type);
            this->onBegin(*composer);
        }

        bool advance()
        {
            if( ! _current )
                return true;

            bool finished = this->onAdvance();
            if(finished)
            {
                delete _current;
                _current = 0;
            }

            return finished;
        }

        void finish()
        {
            if(_context)
                _context->fixup();
        }

    protected:
        virtual void onBegin(IComposer& deser) = 0;

        virtual bool onAdvance() = 0;

    protected:
        virtual void get(IComposer& deser) = 0;

    private:
        SerializationContext* _context;
        IComposer* _current;
};

} // namespace Pt

#endif
