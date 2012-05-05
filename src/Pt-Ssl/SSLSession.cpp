/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2012 by Marc Duerner
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

#include <Pt/Ssl/SSLSession.h>
#include <fstream>
#include <openssl/ssl.h>

namespace Pt {

namespace Ssl {

class SessionImpl : private NonCopyable
{
    public:
        SessionImpl(ssl_session_st* sess)
        : _sess(sess)
        , _refs(1)
        {
        }

        ~SessionImpl()
        { 
            if(_sess)
                SSL_SESSION_free(_sess); 
        }

        void addRef()
        {
            ++_refs;
        }

        void unref()
        {
            if(--_refs == 0)
                delete this;
        }

        ssl_session_st* sslSession() const
        { return _sess; }

    private:
        ssl_session_st* _sess;
        unsigned _refs;
};

Session::Session()
: _impl(0)
{}


Session::Session(ssl_session_st* sess)
: _impl( new SessionImpl(sess) )
{
}


Session::Session(const Session& sess)
: _impl(sess._impl)
{
    if(_impl)
        _impl->addRef();
}


Session::~Session()
{ 
    if(_impl)
        _impl->unref();
}


Session& Session::operator=(const Session& sess)
{
    if(_impl)
        _impl->unref();

    _impl = sess._impl;
    _impl->addRef();

    return *this;
}


ssl_session_st* Session::impl() const
{
    if( ! _impl) 
        return 0;

    return _impl->sslSession();;
}

} // namespace Ssl

} // namespace Pt

