/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#ifndef PT_SSL_API_CERTIFICATES_H
#define PT_SSL_API_CERTIFICATES_H

/** @addtogroup Pt-Ssl-Certificates

    @brief Load and inspect X509 certificates.

    SSL/TLS work starts with certificates. A server presents an
    identity, a client may present one, and both sides usually have
    a list of trusted CA certificates with which they verify the
    peer. Those certificates, and the private keys that belong to
    them, are loaded into a %CertificateStore.

    PKCS12 is the usual container: one file or memory buffer can
    hold several certificates and their keys, often protected by a
    password. PEM is the other encoding %loadPem() accepts from
    memory. After a load, the store owns the certificates. A private
    key stays attached to its certificate and is not exposed by any
    function; selecting a certificate later as a context identity
    selects that key as well.

    Certificates in the store are inspected by subject. %subject()
    on a %Certificate is the subject string, %findCertificate()
    returns a certificate whose subject contains a substring or a
    null pointer, and %getCertificate() does the same search and
    throws %InvalidCertificate when nothing matches. %begin() and
    %end() walk the store. The store must outlive any reference or
    pointer taken from it.

    The example loads PKCS12 data, lists subjects, and looks up one
    certificate by name. Using that certificate in a %Context is the
    next chapter.

    @code
    Pt::Ssl::CertificateStore store;

    const char* password = ...;
    std::ifstream ifs("certs.p12");
    store.loadPkcs12(ifs, password);

    Pt::Ssl::CertificateStore::ConstIterator it;
    for(it = store.begin(); it != store.end(); ++it)
    {
        std::cout << it->subject() << std::endl;
    }

    const Pt::Ssl::Certificate* cert = store.findCertificate("Example Server");
    if( ! cert)
        return;
    @endcode
*/

#endif
