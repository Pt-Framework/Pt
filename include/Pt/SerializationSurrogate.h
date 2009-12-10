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
#ifndef Pt_SerializationSurrogate_h
#define Pt_SerializationSurrogate_h

#include <Pt/Api.h>
#include <Pt/SerializationContext.h>

namespace Pt {

class SerializationContext;
class SerializationInfo;

// class SerializationSurrogate
// {
//     public:
//         virtual ~SerializationSurrogate()
//         {}

//         virtual void pack(SerializationInfo& it) const = 0;

//         virtual void unpack(SerializationInfo& to, const SerializationInfo& from) const = 0;

//     protected:
//         SerializationSurrogate()
//         {}
// };

class SerializationSurrogate
{
    public:
        typedef void (*Deflate)(SerializationInfo& si);

        typedef void (*Inflate)(SerializationInfo& to, const SerializationInfo& from);

    public:
        SerializationSurrogate()
        : _si(0)
        , _deflate(0)
        , _inflate(0)
        { }

        SerializationSurrogate(Deflate def, Inflate inf)
        : _si(0)
        , _deflate(def)
        , _inflate(inf)
        { }

        SerializationSurrogate(const SerializationSurrogate& sp)
        : _si(0)
        , _deflate(sp._deflate)
        , _inflate(sp._inflate)
        { }

        ~SerializationSurrogate()
        {
            if( _si && _si->context() )
            {
                _si->context()->push(_si);
            }
        }

        SerializationSurrogate& operator=(const SerializationSurrogate& sp)
        {
            _si = 0;
            _deflate = sp._deflate;
            _inflate = sp._inflate;
            return *this;
        }

        void deflate(SerializationInfo& si) const
        {
            if(_deflate)
            {
                _deflate(si);
            }
        }


        const SerializationInfo& inflate(const SerializationInfo& from)
        {
            if( ! _inflate )
            {
                return from;
            }

            if(_si == 0)
            {
                _si = from.context()->get();
                _inflate(*_si, from);
            }

            return *_si;
        }

    private:
        SerializationInfo* _si;
        Deflate _deflate;
        Inflate _inflate;
};

} // namespace Pt

#endif
