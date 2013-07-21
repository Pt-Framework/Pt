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
 
#ifndef PT_SSL_CERTIFICATELISTIMPL_H
#define PT_SSL_CERTIFICATELISTIMPL_H

#include "OpenSsl.h"
#include <Pt/Ssl/CertificateList.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/Atomicity.h>
#include <cassert>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#ifdef __APPLE__
#include <Pt/Base64Codec.h>
#include <Pt/TextStream.h>
#import <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFDictionary.h>
#include <string>
#include <sstream>
#endif

namespace Pt {

namespace Ssl {

#ifdef __APPLE__

class CertificateImpl2
{
    public:
        explicit CertificateImpl2(SecCertificateRef cert)
        : _cert(cert)
        , _refs(1)
        {
            assert(_cert);
        }

        CertificateImpl2(const char* der, size_t len)
        : _cert(0)
        , _refs(1)
        {
            CFDataRef data =
                CFDataCreate(NULL, reinterpret_cast<const UInt8*>(der), len);

            _cert = SecCertificateCreateWithData(NULL, data);
                
            CFRelease(data);
        }

        ~CertificateImpl2()
        {
            CFRelease(_cert);
        }

        void ref()
        { atomicIncrement(_refs); }

        int unref()
        { return atomicDecrement(_refs); }

        int serialNumber() const
        {
            CFDataRef data = SecCertificateCopySerialNumber(_cert, NULL);
            
            if( ! data )
                return 0;
                
            const size_t sz = sizeof(int); 
            UInt8 buf[sz];
            CFDataGetBytes(data, CFRangeMake(0, sz), buf);
            CFRelease(data);
            
            int n = 0;
            memcpy(&n, buf, sz);
            return n;
        }

        std::string issuer() const
        {
            return "";
        }

        std::string subject() const
        {
            return "";
        }
        
        std::string notBefore() const
        {
            return "";
        }

        std::string notAfter() const
        {
            return "";
        }

        PublicKey publicKey() const
        {
            
            return PublicKey(0);
        }

    private:
        SecCertificateRef _cert;
        Pt::atomic_t _refs;
};

class CertificateListImpl2
{
    public:
        CertificateListImpl2()
        { }

        CertificateListImpl2(const CertificateListImpl2& list)
        { _certificates = list._certificates; }

        ~CertificateListImpl2()
        { clear(); }

        void push_back(const Certificate& cert)
        { _certificates.push_back(cert); }

        void clear()
        { _certificates.clear(); }

        size_t size() const
        { return _certificates.size(); }

        bool empty() const
        { return _certificates.empty(); }

        Certificate* begin()
        { 
            return _certificates.empty() ? 0 : &_certificates[0]; 
        }

        Certificate* end()
        { 
            return _certificates.empty() ? 0 : &_certificates[0] + _certificates.size(); 
        }

        const Certificate* begin() const
        { 
            return _certificates.empty() ? 0 : &_certificates[0]; 
        }

        const Certificate* end() const
        { 
            return _certificates.empty() ? 0 : &_certificates[0] + _certificates.size(); 
        }

        void fromPem(const char* pemData, size_t len)
        {
            std::istringstream iss( std::string(pemData, len) );
            // alternatively:
            // iss.write(data, len);
            // is.seekg(0, iss.beg);
            BasicTextIStream<char, char> b64conv( iss, new Base64Codec() );

            char rbuf[255];
            std::string data;
            while( b64conv ) 
            {
                b64conv.read(rbuf, sizeof(rbuf));
                size_t count = size_t( b64conv.gcount() );
                data.append(rbuf, count);
            }

            Certificate cert( data.c_str(), data.size() );
            this->push_back(cert);
        }   

    private:
        std::vector<Certificate> _certificates;
};

#endif
    
class CertificateImpl
{
    public:
        explicit CertificateImpl(x509_st* x509)
        : _x509(x509)
        , _refs(1)
        {
            assert(_x509);
        }

        CertificateImpl(const char* data, size_t len)
        : _x509(0)
        , _refs(1)
        {
            BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

            // Try to read/parse DER encoded certificate
            _x509 = d2i_X509_bio(in.get(), 0);
            if( ! _x509)
                throw InvalidCertificate("invalid DER certificate");
        }

        ~CertificateImpl()
        {
            X509_free(_x509);
        }

        void ref()
        { atomicIncrement(_refs); }

        int unref()
        { return atomicDecrement(_refs); }

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
            return toString( X509_get_subject_name(_x509) );
        }
        
        std::string notBefore() const
        {
            return toString( X509_get_notBefore(_x509) );
        }

        std::string notAfter() const
        {
            return toString( X509_get_notAfter(_x509) );
        }

        PublicKey publicKey() const
        {
            EVP_PKEY* pkey = X509_get_pubkey( _x509 );
            return PublicKey(pkey);
        }

        x509_st* getX509() const
        { return _x509; }

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
        x509_st* _x509;
        Pt::atomic_t _refs;
};


class CertificateListImpl
{
    public:
        CertificateListImpl()
        { }

        CertificateListImpl(const CertificateListImpl& list)
        { _certificates = list._certificates; }

        ~CertificateListImpl()
        { clear(); }

        void push_back(const Certificate& cert)
        { _certificates.push_back(cert); }

        void clear()
        { _certificates.clear(); }

        size_t size() const
        { return _certificates.size(); }

        bool empty() const
        { return _certificates.empty(); }

        Certificate* begin()
        { 
            return _certificates.empty() ? 0 : &_certificates[0]; 
        }

        Certificate* end()
        { 
            return _certificates.empty() ? 0 : &_certificates[0] + _certificates.size(); 
        }

        const Certificate* begin() const
        { 
            return _certificates.empty() ? 0 : &_certificates[0]; 
        }

        const Certificate* end() const
        { 
            return _certificates.empty() ? 0 : &_certificates[0] + _certificates.size(); 
        }
        
        //For PEM we use:
        //   PEM_read_PUBKEY
        //   PEM_read_bio_PrivateKey
        //
        //For reading  ASN1 (DER) we use:
        //  d2i_PUBKEY_bio
        //  d2i_PrivateKey_bio
        //
        //For writing, I believe the functions are:
        //   PEM_write_bio_PUBKEY
        //   PEM_write_bio_PrivateKey
        //   i2d_PUBKEY_bio
        //   i2d_PrivateKey_bio
        
        void fromPem(const char* data, size_t len)
        {
            this->clear();

            BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

            // Try to read/parse the CA X509 certificates
            while(true) 
            {
                // Read the certificate
                X509AutoPtr x509 ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
                if( ! x509) 
                  break;

                Certificate cert( new CertificateImpl(x509.get()) );
                this->push_back(cert);
                
                x509.release();
            }
        }

    private:
        std::vector<Certificate> _certificates;
};

} // namespace Ssl

} // namespace Pt

#endif
