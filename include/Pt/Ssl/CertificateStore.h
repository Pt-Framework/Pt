/*
 * Copyright (C) 2010-2013 by Marc Duerner
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
 
#ifndef PT_SSL_CERTIFICATESTORE_H
#define PT_SSL_CERTIFICATESTORE_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Certificate.h>
#include <string>
#include <iosfwd>
#include <cstddef>

namespace Pt {

namespace Ssl {

/** @brief Store for X509 certificates and their keys.

    %CertificateStore is the owner of the certificates the group
    described. PKCS12 data is loaded from an iostream or from a
    memory buffer, and PEM data is loaded from memory. Each load
    adds to the store; it does not replace certificates already
    loaded. Unreadable PKCS12 or PEM data throws
    %InvalidCertificate.

    %findCertificate() searches for a subject substring and returns
    a pointer the store still owns, or a null pointer.
    %getCertificate() is the same search and throws
    %InvalidCertificate when nothing matches. %size(), %begin() and
    %end() inspect the current contents.

    A %Certificate reference or pointer from this store is valid
    only while the store exists and still holds that certificate.

    @ingroup Pt-Ssl-Certificates
*/
class PT_SSL_API CertificateStore
{
    public:
        class ConstIterator;

    public:
        /** @brief Creates an empty certificate store.
        */
        CertificateStore();

        /** @brief Destroys the store and its certificates.
        */
        ~CertificateStore();

        /** @brief Loads PKCS12 certificates and keys from @a is.

            @throw %InvalidCertificate if the PKCS12 data is invalid.
        */
        void loadPkcs12(std::istream& is, const char* passwd);

        /** @brief Loads PKCS12 certificates and keys from memory.

            @throw %InvalidCertificate if the PKCS12 data is invalid.
        */
        void loadPkcs12(const char* data, std::size_t len, const char* passwd);

        /** @brief Loads PEM certificates and keys from memory.

            @throw %InvalidCertificate if the PEM data is invalid.
        */
        void loadPem(const char* data, std::size_t len, const char* passwd);

        /** @brief Returns a certificate whose subject contains @a subject.

            Returns a null pointer if no certificate was found.
        */
        const Certificate* findCertificate(const std::string& subject);

        /** @brief Returns a certificate whose subject contains @a subject.

            @throw %InvalidCertificate if no certificate was found.
        */
        const Certificate& getCertificate(const std::string& subject);

        /** @brief Returns the number of certificates in the store.
        */
        std::size_t size() const;

        /** @brief Returns an iterator to the first certificate.
        */
        ConstIterator begin() const;

        /** @brief Returns an iterator to one past the last certificate.
        */
        ConstIterator end() const;

    private:
        class CertificateStoreImpl* _impl;
};

/** @brief Iterator to a certificate in the store.
*/
class PT_SSL_API CertificateStore::ConstIterator
{
    public:
        //! @brief Default constructor.
        ConstIterator();

        //! @brief Copy constructor.
        ConstIterator(const ConstIterator& other);

        //! @internal
        explicit ConstIterator(Certificate* const* cert);

        //! @brief Assignment operator.
        ConstIterator& operator=(const ConstIterator& other);

        //! @brief Advance iterator position.
        ConstIterator& operator++();

        //! @brief Access value.
        const Certificate& operator*() const;

        //! @brief Access value.
        const Certificate* operator->() const;

        //! @brief Comparison operator.
        bool operator!=(const ConstIterator& other) const;

        //! @brief Comparison operator.
        bool operator==(const ConstIterator& other) const;

    private:
        Certificate* const* _cert;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CERTIFICATESTOREIMPL_H
