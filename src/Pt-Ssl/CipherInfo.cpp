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

#include <Pt/Ssl/CipherInfo.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace Pt {

namespace Ssl {

const char* Cipher::name() const
{ 
    if(_sslCipher)
        return SSL_CIPHER_get_name(_sslCipher); 

    if(_cipherInfo)
        return _cipherInfo->name();

    return "";
}


CipherInfo::CipherInfo()
: _id(0)
, _bits(0)
, _usedBits(0)
{}


CipherInfo::CipherInfo(unsigned long id, const std::string& strid,
                       const std::string& name, int bits, int usedBits,
                       const std::string& version, const std::string& desc)
: _id(id)
, _strid(strid)
, _name(name)
, _bits(bits)
, _usedBits(usedBits)
, _version(version)
, _desc(desc)
{}


CipherList::CipherList()
: _sslCiphers(0)
{
}


CipherList::CipherList(const CipherList& list)
: _sslCiphers(0)
{
}


CipherList::~CipherList()
{
}


void CipherList::clear()
{ 
    _ciphers.clear();
    _sslCiphers = 0;
}


CipherList& CipherList::operator=(const CipherList& list)
{
    clear();

    if(list._sslCiphers)
    {
        STACK_OF(SSL_CIPHER)* chp = reinterpret_cast<STACK_OF(SSL_CIPHER)*>(list._sslCiphers);

        for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i)
        {
            // Skip if not valid
            const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);
            if( ! c->valid )
                continue;

            // Get the ID and split it
            const unsigned long id  = c->id;
            const int           id0 = (int) (  id >> 24);
            const int           id1 = (int) ( (id >> 16) & 0xFFL );
            const int           id2 = (int) ( (id >>  8) & 0xFFL );
            const int           id3 = (int) (  id        & 0xFFL );

            // Convert the ID to a readable string
            char strid[64];
            if((id & 0xFF000000L) == 0x02000000L)
                sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
            else if((id & 0xFF000000L) == 0x03000000L)
                sprintf(strid, "0x%02X,0x%02X", id2, id3);
            else
                sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);

            // Get some information
            char desc[512];
            SSL_CIPHER_description(c, desc, sizeof(desc));
            const int dlen = strlen(desc);
            if(desc[dlen - 1] == '\n')
                desc[dlen - 1] = 0;

            // Store the chiper information
            int usedBits;
            int bits = SSL_CIPHER_get_bits(c, &usedBits);
            CipherInfo cipher(id, strid, SSL_CIPHER_get_name(c), bits, usedBits, 
                              SSL_CIPHER_get_version(c), desc);
        
            _ciphers.push_back(cipher);
        }
    }

    _ciphers.assign(list._ciphers.begin(), list._ciphers.end());

    return *this;
}


const ssl_cipher_st* CipherList::sslCipher(int n) const
{
    if(_sslCiphers)
    {
        STACK_OF(SSL_CIPHER)* chp = reinterpret_cast<STACK_OF(SSL_CIPHER)*>(_sslCiphers);
        if( n < sk_SSL_CIPHER_num(chp) )
        {
            const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, n);
            return c;
        }
    }

    return 0;
}


const CipherInfo* CipherList::cipherInfo(int n) const
{
    unsigned offset = static_cast<unsigned>(n); 
    if( offset < _ciphers.size() )
    {
        return &_ciphers[offset];
    }

    return 0;
}


CipherIterator CipherList::begin() const
{ 
    if(_sslCiphers)
        return CipherIterator(*this, sslCipher(0), 0);

    return CipherIterator(*this, cipherInfo(0), 0); 
}


CipherIterator CipherList::end() const
{ 
    return CipherIterator(); 
}


CipherIterator::CipherIterator(const CipherList& list, const ssl_cipher_st* cipher, int n)
: _list(&list)
, _n(n)
{
    _cipher.set( _list->sslCipher(_n) );
}


CipherIterator::CipherIterator(const CipherList& list, const CipherInfo* info, int n)
: _list(&list)
, _n(n)
{
    _cipher.set( _list->cipherInfo(_n) );
}


CipherIterator CipherIterator::operator++()
{        
    if( _list->_sslCiphers ) 
        _cipher.set( _list->sslCipher(++_n) );
    else
        _cipher.set( _list->cipherInfo(++_n) );

    return *this;
}

} // namespace Ssl

} // namespace Pt
