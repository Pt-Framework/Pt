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
#ifndef PT_SSL_CERTIFICATELIST_H
#define PT_SSL_CERTIFICATELIST_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/PublicKey.h>
#include <Pt/Ssl/PrivateKey.h>
#include <Pt/NonCopyable.h>
#include <vector>

namespace Pt {

namespace Ssl {

class CertificateImpl;
    
class PT_SSL_API Certificate
{
    public:     
        Certificate();

        Certificate(const char* data, size_t len);

        explicit Certificate(CertificateImpl* impl);

        Certificate(const Certificate& cert);

        ~Certificate();

        Certificate& operator=(const Certificate& cert);

        int serialNumber() const;

        std::string issuer() const;

        std::string subject() const;
          
        std::string notBefore() const;

        std::string notAfter() const;
        
        PublicKey publicKey() const;

        CertificateImpl* impl() const;

    private:
        CertificateImpl* _impl;
};


class Identity
{
    public:
        Identity(const Certificate& cert, const PrivateKey& key);

        ~Identity();

        const Certificate& certificate() const
        { return _cert; }

        const PrivateKey& privateKey() const
        { return _key; }

    private:
        Certificate _cert;
        PrivateKey _key;
        
};


//! \brief Certificate list.
class PT_SSL_API CertificateList
{
    public:
        //! @brief Forward iterator for certificate lists
        class Iterator;

        //! @brief Forward iterator for constant certificate lists
        class ConstIterator;

    public:
        //! \brief Instantiate an empty certificate-list.
        CertificateList();

        CertificateList(const CertificateList& list);

        //! \brief Standard dtor.
        ~CertificateList();

        CertificateList& operator=(const CertificateList& list);

        //! \brief Read certifictates in PEM format.
        void fromPem(const char* data, size_t len);

        //! \brief Read certifictates in PEM format from a stream.
        void fromPem(std::istream& is);

        //! \brief Read certifictates in PEM format from a file.
        void fromPemFile(const char* path);

        //! \brief Clear (delete) any loaded certificate.
        void clear();

        void push_back(const Certificate& cert);

        bool empty() const;

        size_t size() const;

        Iterator begin();
        
        Iterator end();

        ConstIterator begin() const;
        
        ConstIterator end() const;

    private:
        class CertificateListImpl* _impl;
};


typedef std::vector<Identity> IdentityList;


class PT_SSL_API CertificateStore : private NonCopyable
{
    public:
        CertificateStore();

        ~CertificateStore();

        const IdentityList& identities()
        { return _identities; }

        const CertificateList certificates()
        { return _certificates; }

        void addPem(const char* data, size_t len, const std::string& passwd);

        void loadPkcs12(const char* data, size_t len, const char* passwd);

        void loadPkcs12(std::istream& is, const char* passwd);

    private:
        IdentityList _identities;
        CertificateList _certificates;
};


//! @brief Forward iterator for certificate lists
class CertificateList::Iterator
{
    public:
        Iterator()
        : _c(0)
        {}

        Iterator(const Iterator& other)
        : _c(other._c)
        {}

        explicit Iterator(Certificate* c)
        : _c(c)
        {}

        Iterator& operator=(const Iterator& other)
        {
            _c = other._c;
            return *this;
        }

        Iterator& operator++()
        {
            ++_c;
            return *this;
        }

        Certificate& operator*() const
        { return *_c; }

        Certificate* operator->() const
        { return _c; }

        bool operator!=(const Iterator& other) const
        { return _c != other._c; }

        bool operator==(const Iterator& other) const
        { return _c == other._c; }

    private:
        Certificate* _c;
};

//! @brief Forward iterator for certificate lists
class CertificateList::ConstIterator
{
    public:
        ConstIterator()
        : _c(0)
        {}

        ConstIterator(const ConstIterator& other)
        : _c(other._c)
        {}

        explicit ConstIterator(const Certificate* c)
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

        const Certificate& operator*() const
        { return *_c; }

        const Certificate* operator->() const
        { return _c; }

        bool operator!=(const ConstIterator& other) const
        { return _c != other._c; }

        bool operator==(const ConstIterator& other) const
        { return _c == other._c; }

    private:
        const Certificate* _c;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CERTIFICATELIST_H
