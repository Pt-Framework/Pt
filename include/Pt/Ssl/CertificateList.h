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
#include <Pt/Ssl/SSLCertificateInfo.h>
#include <Pt/Ssl/PublicKey.h>
#include <Pt/NonCopyable.h>
#include <vector>

struct x509_st;

namespace Pt {

namespace Ssl {

class PT_SSL_API Certificate : private NonCopyable
{
    public:     
        explicit Certificate(x509_st* x509);

        Certificate(const Certificate& cert);

        ~Certificate();

        Certificate& operator=(const Certificate& cert);

        PublicKey publicKey() const;

        x509_st* getX509() const;

    private:
        class CertificateImpl* _impl;
};


//! \brief Certificate list.
class PT_SSL_API CertificateList
{
    public:
        //! @brief Forward iterator for certificate lists
        class Iterator;

    public:
        //! \brief Instantiate an empty certificate-list.
        CertificateList();

        CertificateList(const CertificateList& list);

        //! \brief Standard dtor.
        ~CertificateList();

        CertificateList& operator=(const CertificateList& list);

        //! \brief Load certificate from the given data.
        void loadFromString(const std::string& certData);

        //! \brief Load certificate from the given file.
        void loadFromFile(const std::string& fileName);

        //! \brief Clear (delete) any loaded certificate.
        void clear();

        bool empty() const;

        size_t size() const;

        Iterator begin() const;
        
        Iterator end() const;

    private:
        class CertificateListImpl* _impl;
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

        explicit Iterator(const Certificate* c)
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

        const Certificate& operator*() const
        { return *_c; }

        const Certificate* operator->() const
        { return _c; }

        bool operator!=(const Iterator& other) const
        { return _c != other._c; }

        bool operator==(const Iterator& other) const
        { return _c == other._c; }

    private:
        const Certificate* _c;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CERTIFICATELIST_H
