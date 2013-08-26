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

namespace Pt {

namespace Ssl {

class PT_SSL_API CertificateStore
{
    public:
        class ConstIterator;

    public:
        CertificateStore();

        ~CertificateStore();

        void loadPkcs12(std::istream& is, const char* passwd);

        void loadPkcs12(const char* data, size_t len, const char* passwd);

        const Certificate* findCertificate(const std::string& subject);

        const Certificate& getCertificate(const std::string& subject);

        std::size_t size() const;

        ConstIterator begin() const;

        ConstIterator end() const;

    private:
        class CertificateStoreImpl* _impl;
};

class PT_SSL_API CertificateStore::ConstIterator
{
    public:
        ConstIterator();

        ConstIterator(const ConstIterator& other);

        explicit ConstIterator(Certificate* const* cert);

        ConstIterator& operator=(const ConstIterator& other);

        ConstIterator& operator++();

        const Certificate& operator*() const;

        const Certificate* operator->() const;

        bool operator!=(const ConstIterator& other) const;

        bool operator==(const ConstIterator& other) const;

    private:
        Certificate* const* _cert;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CERTIFICATESTOREIMPL_H