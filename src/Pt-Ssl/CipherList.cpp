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

#include <Pt/Ssl/CipherList.h>
#include <Pt/System/Logger.h>
#include <cassert>
#include <openssl/ssl.h>
#include <openssl/err.h>

log_define("Pt.Ssl.CipherList")

namespace Pt {

namespace Ssl {

struct CipherData
{
    CipherData()
    : id(0)
    , bits(0)
    , usedBits(0)
    {}

    CipherData(const SSL_CIPHER* c)
    {
        // Get the numeric ID and split it
        id  = c->id;
        const int id0 = (int) (  id >> 24);
        const int id1 = (int) ( (id >> 16) & 0xFFL );
        const int id2 = (int) ( (id >>  8) & 0xFFL );
        const int id3 = (int) (  id        & 0xFFL );

        // build string id
        char strid[64];
        if((id & 0xFF000000L) == 0x02000000L)
            sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
        else if((id & 0xFF000000L) == 0x03000000L)
            sprintf(strid, "0x%02X,0x%02X", id2, id3);
        else
            sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);

        stringId = strid;

        // cipher description
        char desc[512];
        SSL_CIPHER_description(c, desc, sizeof(desc));
        const int dlen = strlen(desc);
        if(desc[dlen - 1] == '\n')
            desc[dlen - 1] = 0;

        desciption = desciption;

        // other cipher fields
        bits = SSL_CIPHER_get_bits(c, &usedBits);
        name = SSL_CIPHER_get_name(c);
        version = SSL_CIPHER_get_version(c);
    }

    unsigned long id;
    std::string   stringId;
    std::string   name;
    int           bits;
    int           usedBits;
    std::string   version;
    std::string   desciption;
};




Cipher::~Cipher()
{}


const char* Cipher::name() const
{
    if(_sslCipher)
    {
        if( ! _sslCipher->valid )
            return "INVALID";

        return SSL_CIPHER_get_name(_sslCipher);
    }

    if(_cipherData)
        return _cipherData->name.c_str();

    log_warn("using uninitialized cipher");
    return "";
}




CipherList::CipherList()
: _sslCiphers(0)
{
}


CipherList::CipherList(const CipherList& list)
: _sslCiphers(0)
{
    *this = list;
}


CipherList::~CipherList()
{
}


CipherList& CipherList::operator=(const CipherList& list)
{
    log_trace("CipherList::operator=()");
    clear();

    if(list._sslCiphers)
    {
        STACK_OF(SSL_CIPHER)* chp = reinterpret_cast<STACK_OF(SSL_CIPHER)*>(list._sslCiphers);
        log_trace(sk_SSL_CIPHER_num(chp) << " ciphers to copy");

        for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i)
        {
            // Skip if not valid
            const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);
            if( ! c->valid )
                continue;

            CipherData cipher(c);
            log_trace("copied cipher: " << cipher.name );
            _ciphers.push_back(cipher);
        }
    }
    else
    {
        log_trace("assigned " << list._ciphers.size() << " ciphers");
        _ciphers.assign(list._ciphers.begin(), list._ciphers.end());
    }

    return *this;
}


void CipherList::clear()
{ 
    _ciphers.clear();
    _sslCiphers = 0;
}


size_t CipherList::size() const
{
    size_t n = _ciphers.size();

    if(_sslCiphers)
    {
        STACK_OF(SSL_CIPHER)* chp = reinterpret_cast<STACK_OF(SSL_CIPHER)*>(_sslCiphers);
        n = static_cast<size_t>( sk_SSL_CIPHER_num(chp) );
    }

    return n;
}


const ssl_cipher_st* CipherList::sslCipher(int n) const
{
    log_trace("CipherList::sslCipher(" << n << ")");

    STACK_OF(SSL_CIPHER)* chp = reinterpret_cast<STACK_OF(SSL_CIPHER)*>(_sslCiphers);
    if( chp && n < sk_SSL_CIPHER_num(chp) )
    {
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, n);
        log_trace("CipherList::sslCipher() -> " << c);
        return c;
    }

    log_trace("CipherList::sslCipher() -> " << 0);
    return 0;
}


const CipherData* CipherList::cipherInfo(int n) const
{
    log_trace("CipherList::cipherInfo(" << n << ")");

    unsigned offset = static_cast<unsigned>(n); 
    if( offset < _ciphers.size() )
    {
        const CipherData* c = &_ciphers[offset];
        log_trace("CipherList::cipherInfo() -> " << c);
        return c;
    }

    log_trace("CipherList::cipherInfo() -> " << 0);
    return 0;
}


void CipherList::setRef(void* sslCiphers)
{ 
    _sslCiphers = sslCiphers; 
}



CipherList::Iterator::Iterator(const CipherList& list, int n, bool move)
: _list(&list)
, _n(n)
, _move(move)
{
    if( _list->_sslCiphers ) 
        _cipher.setRef( _list->sslCipher(_n) );
    else
        _cipher.setRef(_list->cipherInfo(_n) );
}


CipherList::Iterator::Iterator(const Iterator& other)
: _list(other._list)
, _n(other._n)
, _move(false)
{
    if(other._move)
    {
        _cipher.moveFrom(other._cipher);
    }
    else
    {
        if( _list->_sslCiphers ) 
            _cipher.setRef( _list->sslCipher(_n) );
        else
            _cipher.setRef( _list->cipherInfo(_n) );
    }
}


CipherList::Iterator& CipherList::Iterator::operator=(const Iterator& other)
{ 
    _list = other._list; 
    _n = other._n;
    _move = false;

    if(other._move)
    {
        _cipher.moveFrom(other._cipher);
    }
    else
    {
        if( _list->_sslCiphers ) 
            _cipher.setRef( _list->sslCipher(_n) );
        else
            _cipher.setRef( _list->cipherInfo(_n) );
    }

    return *this; 
}


CipherList::Iterator& CipherList::Iterator::operator++()
{        
    ++_n;

    if( _list->_sslCiphers ) 
        _cipher.setRef( _list->sslCipher(_n) );
    else
        _cipher.setRef( _list->cipherInfo(_n) );

    return *this;
}

} // namespace Ssl

} // namespace Pt
