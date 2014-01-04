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
 
#ifndef PT_SSL_CERTIFICATEIMPL_H
#define PT_SSL_CERTIFICATEIMPL_H

#include "OpenSsl.h"
#include <cassert>

namespace Pt {

namespace Ssl {

class CertificateImpl
{
    public:
        explicit CertificateImpl(X509* x509, EVP_PKEY* pkey = 0)
        : _x509(x509)
        , _pkey(pkey)
        {
            assert(_x509);
        }

        //CertificateImpl(const char* data, size_t len)
        //: _x509(0)
        //, _refs(1)
        //{
        //    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

        //    // Try to read/parse DER encoded certificate
        //    _x509 = d2i_X509_bio(in.get(), 0);
        //    if( ! _x509)
        //        throw InvalidCertificate("invalid DER certificate");
        //}

        ~CertificateImpl()
        {
            if(_pkey)
                EVP_PKEY_free(_pkey);

            X509_free(_x509);
        }

        int serialNumber() const
        {
            return ASN1_INTEGER_get( X509_get_serialNumber(_x509) );
        }

        std::string issuer() const
        {
            return toString( X509_get_issuer_name(_x509) );
        }

        std::string subject() const
        {
            char buf[255];

            std::string s;

            X509_NAME* subject = X509_get_subject_name(_x509);

            if( 0 <= X509_NAME_get_text_by_NID(subject, NID_organizationName, buf, sizeof(buf)) ) 
            {
                s += buf;
            }
            else
            {
                s += "Unkown Organization";
            }

            s += ", ";

            if( 0 <= X509_NAME_get_text_by_NID(subject, NID_commonName, buf, sizeof(buf)) ) 
            {
                s += buf;
            }
            else
            {
                s += "Unknown Name";
            }

            // NID_countryName
            // NID_localityName
            // NID_organizationalUnitName
            // NID_stateOrProvinceName

            return s;

            //return toString( X509_get_subject_name(_x509) );
        }

        X509* x509() const
        { return _x509; }

        EVP_PKEY* pkey() const
        { return _pkey; }

    private:
        static std::string toString(const X509_NAME* val)
        {
            int len = 0;
            char buf[1024];
            
            BioAutoPtr out( BIO_new(BIO_s_mem()) );
            if( X509_NAME_print( out.get(), (X509_NAME*) val, 0) ) 
            {
                len = BIO_read( out.get(), buf, sizeof(buf) );
            }
            
            return std::string(buf, len);
        }

        static std::string toString(ASN1_TIME* val)
        {
            int len = 0;
            char buf[1024];

            BioAutoPtr out( BIO_new(BIO_s_mem()) );
            if( ASN1_TIME_print( out.get(), val) )
            {
                len = BIO_read( out.get(), buf, sizeof(buf) );
            }

            return std::string(buf, len);
        }

    private:
        X509* _x509;
        EVP_PKEY* _pkey;
};

} // namespace Ssl

} // namespace Pt

#endif
