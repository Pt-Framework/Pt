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

#ifndef PT_SSL_PRIVATEKEYIMPL_H
#define PT_SSL_PRIVATEKEYIMPL_H

#include "OpenSsl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/NonCopyable.h>
#include <Pt/Atomicity.h>
#include <iostream>
#include <fstream>

namespace Pt {

namespace Ssl {

class PrivateKeyImpl : private Pt::NonCopyable
{
    public:
        PrivateKeyImpl(const std::string& password)
        : _pswd(password)
        , _pkey(0)
        , _refs(1)
        { }

        PrivateKeyImpl(evp_pkey_st* pkey)
        : _pswd()
        , _pkey(pkey)
        , _refs(1)
        { }
        
        ~PrivateKeyImpl()
        {
            if(_pkey)
                EVP_PKEY_free(_pkey);
        }

        void ref()
        { atomicIncrement(_refs); }

        int unref()
        { return atomicDecrement(_refs); }

        void fromPem(const char* data, size_t len);

        void fromPem(std::istream& is);

        void fromPemFile(const char* path);

        void toPem(std::ostream& os) const;

        static int passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata);

        evp_pkey_st* evp()
        { return _pkey; }

    private:
        const std::string  _pswd;
        evp_pkey_st* _pkey;
        Pt::atomic_t _refs;
};


inline void PrivateKeyImpl::fromPem(const char* data, size_t len)
{
    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

    // Try to read/parse the private key
    _pkey = PEM_read_bio_PrivateKey(in.get(), 0, &PrivateKeyImpl::passwordCallback, (void*) &_pswd);
    if(!_pkey)
        throw InvalidKey("Could not read/parse/decode private-key data!");
}


inline void PrivateKeyImpl::fromPem(std::istream& is)
{
    char rbuf[4096];
    const std::streamsize rbufSize = sizeof(rbuf);
    std::string data;
    while( is ) 
    {
        is.read(rbuf, rbufSize);
        size_t count = size_t( is.gcount() );
        data.append(rbuf, count);
    }

    fromPem( data.c_str(), data.size() );
}


inline void PrivateKeyImpl::fromPemFile(const char* path)
{
    std::ifstream ifs(path, std::ios::binary);
    fromPem(ifs);
}


inline void PrivateKeyImpl::toPem(std::ostream& os) const
{
    if( ! _pkey)
        return;
    
    BioAutoPtr out( BIO_new(BIO_s_mem()) );

    int ret = PEM_write_bio_PrivateKey(out.get(), _pkey, EVP_des_ede3_cbc(), NULL, 0, 
                                       &PrivateKeyImpl::passwordCallback, (void*) &_pswd);
    if( ! ret)
        throw InvalidKey("Could not write key in pem format");

    char* data = 0;
    long len = BIO_get_mem_data(out.get(), &data);
    os.write(data, len);
}


inline int PrivateKeyImpl::passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata)
{
    // Get the password
    const std::string& password = *((std::string*) userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if((unsigned) num < password.length() + 1) 
        return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &password[0]);
    return password.length();
}

} // namespace Ssl

} // namespace Pt

#endif
