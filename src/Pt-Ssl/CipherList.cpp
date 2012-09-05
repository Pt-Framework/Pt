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

class CipherData
{
    public:
        CipherData()
        : bits(0)
        , usedBits(0)
        {}

        CipherData(const SSL_CIPHER* c)
        {
            // Get the numeric ID and split it
            //id  = c->id;
            //const int id0 = (int) (  id >> 24);
            //const int id1 = (int) ( (id >> 16) & 0xFFL );
            //const int id2 = (int) ( (id >>  8) & 0xFFL );
            //const int id3 = (int) (  id        & 0xFFL );

            // build string id
            //char strid[64];
            //if((id & 0xFF000000L) == 0x02000000L)
            //    sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
            //else if((id & 0xFF000000L) == 0x03000000L)
            //    sprintf(strid, "0x%02X,0x%02X", id2, id3);
            //else
            //    sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);

            //stringId = strid;

            // cipher description
            //char desc[512];
            //SSL_CIPHER_description(c, desc, sizeof(desc));
            //const int dlen = strlen(desc);
            //if(desc[dlen - 1] == '\n')
            //    desc[dlen - 1] = 0;

            //desciption = desciption;

            // other cipher fields
            bits = SSL_CIPHER_get_bits(c, &usedBits);
            name = SSL_CIPHER_get_name(c);
            version = SSL_CIPHER_get_version(c);
        }

        std::string   name;
        std::string   version;
        int           bits;
        int           usedBits;
};


Cipher::Cipher()
: _cipherData( new CipherData() )
{}


Cipher::Cipher(const SSL_CIPHER* c)
: _cipherData( new CipherData(c) )
{
}


Cipher::Cipher(const Cipher& ciph)
: _cipherData( new CipherData() )
{
    const CipherData* other = ciph._cipherData;
    _cipherData = new CipherData(*other);
}


Cipher::~Cipher()
{
    delete _cipherData;
}

        
Cipher& Cipher::operator=(const Cipher& ciph)
{
    *_cipherData = *(ciph._cipherData);
    return *this;
}


const char* Cipher::name() const
{
    return _cipherData->name.c_str();
}


const char* Cipher::version() const
{
    return _cipherData->version.c_str();
}


int Cipher::bits() const
{
    return _cipherData->bits;
}


int Cipher::usedBits() const
{
    return _cipherData->usedBits;
}




class CipherListImpl
{
    public:
        CipherListImpl()
        { }

        ~CipherListImpl()
        { clear(); }

        void push_back(const Cipher& cert)
        { _ciphers.push_back(cert); }

        void clear()
        { _ciphers.clear(); }

        size_t size() const
        { return _ciphers.size(); }

        bool empty() const
        { return _ciphers.empty(); }

        const Cipher* begin()
        { 
            return _ciphers.empty() ? 0 : &_ciphers[0]; 
        }

        const Cipher* end() const
        { 
            return _ciphers.empty() ? 0 : &_ciphers[0] + _ciphers.size(); 
        }

    private:
        std::vector<Cipher> _ciphers;
};


CipherList::CipherList()
: _impl( new CipherListImpl() )
{
}


CipherList::CipherList(void* sslCiphers)
: _impl( new CipherListImpl() )
{
    STACK_OF(SSL_CIPHER)* chp = reinterpret_cast<STACK_OF(SSL_CIPHER)*>(sslCiphers);
    for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i)
    {
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);

        // Skip if not valid
        if( c->valid )
        {
          _impl->push_back( Cipher(c) ) ;
        }
    }
}


CipherList::CipherList(const CipherList& list)
: _impl( new CipherListImpl( *(list._impl) ) )
{
}


CipherList::~CipherList()
{
    delete _impl;
}


CipherList::ConstIterator CipherList::begin() const
{ 
    return ConstIterator( _impl->begin() ); 
}

        
CipherList::ConstIterator CipherList::end() const
{ 
    return ConstIterator( _impl->end() ); 
}


bool CipherList::empty() const
{
    return _impl->empty();
}


void CipherList::clear()
{
    _impl->clear();
}

size_t CipherList::size() const
{
    return _impl->size();
}

CipherList& CipherList::operator=(const CipherList& list)
{
    *_impl = *(list._impl);
    return *this;
}

} // namespace Ssl

} // namespace Pt
