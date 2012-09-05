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
#ifndef PT_SSL_CIPHERLIST_H
#define PT_SSL_CIPHERLIST_H

#include <Pt/Ssl/Api.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>

struct ssl_cipher_st;

namespace Pt {

namespace Ssl {

//! @brief Represents a cipher algorithm
class PT_SSL_API Cipher
{
    public:
        Cipher();

        Cipher(const ssl_cipher_st* cipher);

        Cipher(const Cipher& ciph);

        ~Cipher();

        Cipher& operator=(const Cipher& ciph);

        const char* name() const;

        const char* version() const;

        int bits() const;

        int usedBits() const;

    private:
        class CipherData* _cipherData;
};

//! @brief List of cipher alogorithms.
class PT_SSL_API CipherList 
{
    public:
        //! @brief Forward iterator for constant cipher lists.
        class ConstIterator;

    public:
        CipherList();

        CipherList(void* sslCiphers);

        CipherList(const CipherList& list);

        //! \brief Destructor.
        ~CipherList();

        CipherList& operator=(const CipherList& list);

        ConstIterator begin() const;
        
        ConstIterator end() const;

        bool empty() const;

        size_t size() const;

        void clear();

    private:
        class CipherListImpl* _impl;
};

//! @brief Forward iterator for certificate lists
class CipherList::ConstIterator
{
    public:
        ConstIterator()
        : _c(0)
        {}

        ConstIterator(const ConstIterator& other)
        : _c(other._c)
        {}

        explicit ConstIterator(const Cipher* c)
        : _c(c)
        {}

        ConstIterator& operator=(const ConstIterator& other)
        {
            _c = other._c;
            return *this;
        }

        ConstIterator& operator++()
        {
            ++_c;
            return *this;
        }

        const Cipher& operator*() const
        { return *_c; }

        const Cipher* operator->() const
        { return _c; }

        bool operator!=(const ConstIterator& other) const
        { return _c != other._c; }

        bool operator==(const ConstIterator& other) const
        { return _c == other._c; }

    private:
        const Cipher* _c;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CIPHERLIST_H

