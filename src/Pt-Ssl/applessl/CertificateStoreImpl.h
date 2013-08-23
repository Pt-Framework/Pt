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
#ifndef PT_SSL_CERTIFICATESTOREIMPL_H
#define PT_SSL_CERTIFICATESTOREIMPL_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Certificate.h>
#include <Pt/Ssl/CertificateStore.h>
#include <string>
#include <vector>

namespace Pt {

namespace Ssl {

class CertificateStoreImpl
{
    public:
        CertificateStoreImpl();

        ~CertificateStoreImpl();

        std::size_t size() const
        { return _allCerts.size(); }

        CertificateStore::ConstIterator begin() const
        { 
            Certificate* const* cert = _allCerts.empty() ? 0 
                                                         : &_allCerts[0];
            return CertificateStore::ConstIterator( cert); 
        }

        CertificateStore::ConstIterator end() const
        {
            Certificate* const* cert = _allCerts.empty() ? 0 
                                                         : &_allCerts[0] + _allCerts.size();
            return CertificateStore::ConstIterator(cert); 
        }

        void loadPkcs12(const char* data, size_t len, const char* passwd);

        const Certificate* findCertificate(const std::string& subject);

    private:
        std::vector<Certificate*> _allCerts;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_CERTIFICATESTOREIMPL_H
