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
#ifndef PT_SSL_CIPHER_H
#define PT_SSL_CIPHER_H

#include <Pt/Ssl/Api.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>

struct ssl_cipher_st;

namespace Pt {

namespace Ssl {

struct CipherData;

//! @brief Represents a cipher algorithm
class PT_SSL_API Cipher : private NonCopyable
{
    public:
        Cipher()
        : _sslCipher(0)
        , _cipherData(0)
        {}

        ~Cipher();

        const char* name() const;

        //! @internal 
        void setRef(const ssl_cipher_st* c)
        { _sslCipher = c; _cipherData = 0; }

        //! @internal 
        void setRef(const CipherData* c)
        { _sslCipher = 0; _cipherData = c; }

        //! @internal 
        void moveFrom(Cipher& c)
        { 
            _sslCipher = c._sslCipher; 
            _cipherData = c._cipherData; 
        }

    private:
        const ssl_cipher_st* _sslCipher;
        const CipherData* _cipherData;
};

//! @brief List of cipher alogorithms.
class PT_SSL_API CipherList 
{
    public:
        //! @brief Forward iterator for %CipherList
        class PT_SSL_API Iterator
        {
            public:
                Iterator()
                : _list(0)
                , _n(-1)
                , _move(true)
                { }
        
                Iterator(const CipherList& list, int n, bool move = false);
        
                Iterator(const Iterator& other);
                
                ~Iterator()
                { }
        
                Iterator& operator=(const Iterator& other);
        
                Iterator& operator++();
        
                const Cipher& operator*() const
                { return _cipher; }
        
                const Cipher* operator->() const
                { return &_cipher; }
        
                bool operator !=(const Iterator& other) const
                { return _n != other._n; }
        
                bool operator ==(const Iterator& other) const
                { return _n == other._n; }
        
            private:
                const CipherList* _list;
                int _n;
                mutable Cipher _cipher;
                bool _move;
        };

    public:
        CipherList();

        CipherList(const CipherList& list);

        //! \brief Standard dtor.
        ~CipherList();

        CipherList& operator=(const CipherList& list);

        //! \brief Clears the list.
        void clear();

        size_t size() const;

        Iterator begin() const
        { return Iterator(*this, 0, true); }
        
        Iterator end() const
        { return Iterator(*this, size(), true); }

        //! @internal
        void setRef(void* sslCiphers);

    protected:
        const ssl_cipher_st* sslCipher(int n) const;

        const CipherData* cipherInfo(int n) const;

    private:
        std::vector<CipherData> _ciphers;
        void* _sslCiphers;
};

} // namespace Ssl

} // namespace Pt

#endif

