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
#ifndef PT_SSL_CIPHERINFO_H
#define PT_SSL_CIPHERINFO_H

#include <Pt/Ssl/Api.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>
#include <cassert>

struct ssl_cipher_st;

namespace Pt {

namespace Ssl {

class CipherInfo;

class PT_SSL_API Cipher : private NonCopyable
{
    public:
        Cipher()
        {}

        ~Cipher()
        {}

        const char* name() const;

        void set(const ssl_cipher_st* c)
        {_sslCipher = c; _cipherInfo = 0; }

        void set(const CipherInfo* c)
        {_sslCipher = 0; _cipherInfo = c; }

    private:
        const ssl_cipher_st* _sslCipher;
        const CipherInfo* _cipherInfo;
};

//! @brief Provides information about chiphers.
class PT_SSL_API CipherInfo
{
    public:
        //! @brief Default constructor.
        CipherInfo();

        //! @brief Constructs from cipher properties.
        CipherInfo(unsigned long id,
                      const std::string& strid,
                      const std::string& name,
                      int bits,
                      int usedBits,
                      const std::string& version,
                      const std::string& desc);

        //! @brief Returns the numerical ID of the cipher.
        unsigned long id() const
        { return _id; }

        //! @brief Returns the string ID of the cipher.
        const std::string& stringId() const
        { return _strid; }

        //! @brief Returns the name of the cipher.
        const char* name() const
        { return _name.c_str(); }

        //! @brief Returns the number of bits supported by the cipher.
        int bits() const
        { return _bits; }

        //! @brief Returns the number of bits actually used by the cipher.
        int usedBits() const
        { return _usedBits; }

        //! @brief Returns the version of the cipher.
        const std::string& version() const
        { return _version; }

        //! @brief Returns the description of the cipher.
        const std::string& description() const
        { return _desc; }

    private:
        unsigned long _id;
        std::string   _strid;
        std::string   _name;
        int           _bits;
        int           _usedBits;
        std::string   _version;
        std::string   _desc;
};

//! @brief Compares two ciphers.
inline bool operator==(const CipherInfo& a, const CipherInfo& b)
{
    return a.id() == b.id();
}

//! @brief Compares two ciphers.
inline bool operator!=(const CipherInfo& a, const CipherInfo& b)
{
    return a.id() != b.id();
}

//! @brief Compares two ciphers.
inline bool operator<(const CipherInfo& a, const CipherInfo& b)
{
    return a.id() < b.id();
}

class CipherIterator;

//! @brief Cipher list.
class PT_SSL_API CipherList 
{
    friend class CipherIterator;

    public:
        CipherList();

        CipherList(const CipherList& list);

        //! \brief Standard dtor.
        ~CipherList();

        //! \brief Clears the list.
        void clear();

        CipherList& operator=(const CipherList& list);

        CipherIterator begin() const;

        CipherIterator end() const;
   
        const ssl_cipher_st* sslCipher(int n) const;

        const CipherInfo* cipherInfo(int n) const;

        void set(void* sslCiphers)
        { _sslCiphers = sslCiphers; }

    private:
        std::vector<CipherInfo> _ciphers;
        void* _sslCiphers;
};

class CipherIteratorImpl;

class PT_SSL_API CipherIterator
{
    public:
        CipherIterator(const CipherList& list, const ssl_cipher_st* cipher, int n);

        CipherIterator(const CipherList& list, const CipherInfo* info, int n);

        CipherIterator(const CipherIterator& other)
        : _list(other._list)
        , _n(other._n)
        {}

        CipherIterator()
        : _list(0)
        , _n(0)
        { }
        
        ~CipherIterator()
        { }

        CipherIterator operator++();

        const Cipher& operator*() const
        { return _cipher; }

        CipherIterator& operator=(const CipherIterator& other)
        { 
            _list = other._list; 
            _n = other._n; 
            return *this; 
        }

        bool operator !=(const CipherIterator& other) const
        { return _list != other._list || _n != other._n; }

        bool operator ==(const CipherIterator& other) const
        { return _list == other._list && _n == other._n;; }

    private:
        const CipherList* _list;
        int _n;
        Cipher _cipher;
};

} // namespace Ssl

} // namespace Pt

#endif
