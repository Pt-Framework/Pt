/* Copyright (C) 2013 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
