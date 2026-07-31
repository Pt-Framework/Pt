/*
 * Copyright (C) 2026 by Marc Boris Duerner
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

#include "Pkcs12Parser.h"
#include "MbedTls.h"
#include <mbedtls/asn1.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/pkcs12.h>
#include <mbedtls/cipher.h>
#include <mbedtls/md.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <Pt/System/Clock.h>
#include <cstring>
#include <new>
#include <sstream>
#include <vector>

namespace Pt {

namespace Ssl {

// ---------------------------------------------------------------------------
// OIDs (raw value bytes, null-terminated only for sizeof trick)
// ---------------------------------------------------------------------------

static const unsigned char oidPkcs7Data[]      = "\x2A\x86\x48\x86\xF7\x0D\x01\x07\x01";
static const unsigned char oidPkcs7EncData[]   = "\x2A\x86\x48\x86\xF7\x0D\x01\x07\x06";
static const unsigned char oidKeyBag[]         = "\x2A\x86\x48\x86\xF7\x0D\x01\x0C\x0A\x01\x01";
static const unsigned char oidShroudedKeyBag[] = "\x2A\x86\x48\x86\xF7\x0D\x01\x0C\x0A\x01\x02";
static const unsigned char oidCertBag[]        = "\x2A\x86\x48\x86\xF7\x0D\x01\x0C\x0A\x01\x03";
static const unsigned char oidPbes2[]          = "\x2A\x86\x48\x86\xF7\x0D\x01\x05\x0D";
static const unsigned char oidPkcs12Pbe3DES[]  = "\x2A\x86\x48\x86\xF7\x0D\x01\x0C\x01\x03";

static bool oidEq(const mbedtls_asn1_buf& b, const unsigned char* oid, std::size_t n)
{
    return b.len == n && std::memcmp(b.p, oid, n) == 0;
}

#define OID_CMP(b, oid) oidEq((b), (oid), sizeof(oid) - 1)


// ---------------------------------------------------------------------------
// Parse state passed through the call chain
// ---------------------------------------------------------------------------

struct ParseState
{
    mbedtls_pk_context** pkey;
    mbedtls_x509_crt**   cert;
    mbedtls_x509_crt**   ca;
    mbedtls_x509_crt**   caTail;
    const char*          passwd;
    mbedtls_ctr_drbg_context* ctr_drbg;
};


// ---------------------------------------------------------------------------
// Forward declaration
// ---------------------------------------------------------------------------

static bool parseSafeContents(const unsigned char* data, std::size_t len,
                              ParseState& st);


// ---------------------------------------------------------------------------
// CertBag
// ---------------------------------------------------------------------------

static bool parseCertBag(const unsigned char* bagVal, std::size_t len,
                         ParseState& st)
{
    unsigned char* p   = const_cast<unsigned char*>(bagVal);
    const unsigned char* end = bagVal + len;

    // CertBag ::= SEQUENCE { certId OID, certValue [0] EXPLICIT { OCTET STRING } }
    std::size_t seqLen;
    if(mbedtls_asn1_get_tag(&p, end, &seqLen, MBEDTLS_ASN1_CONSTRUCTED |
                                              MBEDTLS_ASN1_SEQUENCE) != 0)
        return false;

    const unsigned char* seqEnd = p + seqLen;

    // certId OID (skip)
    std::size_t oidLen;
    if(mbedtls_asn1_get_tag(&p, seqEnd, &oidLen, MBEDTLS_ASN1_OID) != 0)
        return false;

    p += oidLen;

    // certValue [0] EXPLICIT
    std::size_t ctxLen;
    if(mbedtls_asn1_get_tag(&p, seqEnd, &ctxLen, MBEDTLS_ASN1_CONTEXT_SPECIFIC |
                                                 MBEDTLS_ASN1_CONSTRUCTED | 0) != 0)
        return false;

    const unsigned char* ctxEnd = p + ctxLen;

    // OCTET STRING containing DER cert
    std::size_t derLen;
    if(mbedtls_asn1_get_tag(&p, ctxEnd, &derLen, MBEDTLS_ASN1_OCTET_STRING) != 0)
        return false;

    const unsigned char* derCert = p;

    X509CrtAutoPtr crtPtr( new(std::nothrow) mbedtls_x509_crt() );
    if( ! crtPtr)
        return false;

    mbedtls_x509_crt_init(crtPtr.get());

    if(mbedtls_x509_crt_parse_der(crtPtr.get(), derCert, derLen) != 0)
        return false;

    mbedtls_x509_crt* crt = crtPtr.release();

    if( ! *st.cert)
    {
        *st.cert = crt;
    }
    else
    {
        crt->next = *st.ca;
        *st.ca = crt;
        if( ! *st.caTail)
            *st.caTail = crt;
    }

    return true;
}


// ---------------------------------------------------------------------------
// SafeContents
// ---------------------------------------------------------------------------

static bool parseSafeContents(const unsigned char* data, std::size_t len,
                              ParseState& st)
{
    unsigned char* p   = const_cast<unsigned char*>(data);
    const unsigned char* end = data + len;

    // SafeContents ::= SEQUENCE OF SafeBag
    std::size_t seqLen;
    if(mbedtls_asn1_get_tag(&p, end, &seqLen,
                             MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0)
        return false;

    const unsigned char* seqEnd = p + seqLen;
    while(p < seqEnd)
    {
        // SafeBag ::= SEQUENCE { bagId OID, bagValue [0] EXPLICIT ANY, attrs? }
        std::size_t bagLen;
        if(mbedtls_asn1_get_tag(&p, seqEnd, &bagLen,
                                 MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0)
            return false;

        const unsigned char* bagEnd = p + bagLen;

        // bagId OID
        mbedtls_asn1_buf bagId;
        bagId.tag = MBEDTLS_ASN1_OID;
        if(mbedtls_asn1_get_tag(&p, bagEnd, &bagId.len, MBEDTLS_ASN1_OID) != 0)
            return false;

        bagId.p = p;
        p += bagId.len;

        // bagValue [0] EXPLICIT
        std::size_t bagValLen;
        if(mbedtls_asn1_get_tag(&p, bagEnd, &bagValLen,
                                 MBEDTLS_ASN1_CONTEXT_SPECIFIC |
                                 MBEDTLS_ASN1_CONSTRUCTED | 0) != 0)
            return false;

        unsigned char* bagValStart = p;

        if(OID_CMP(bagId, oidCertBag))
        {
            if( ! parseCertBag(bagValStart, bagValLen, st))
                return false;
        }
        else if(OID_CMP(bagId, oidShroudedKeyBag) || OID_CMP(bagId, oidKeyBag))
        {
            if( ! *st.pkey)
            {
                bool shrouded = OID_CMP(bagId, oidShroudedKeyBag);
                const unsigned char* pwd = shrouded
                    ? reinterpret_cast<const unsigned char*>(st.passwd) : 0;
                std::size_t pwdLen = (shrouded && st.passwd)
                    ? std::strlen(st.passwd) : 0;

                PkAutoPtr pkPtr( new(std::nothrow) mbedtls_pk_context() );
                if( ! pkPtr)
                    return false;

                mbedtls_pk_init(pkPtr.get());

                int ret = mbedtls_pk_parse_key(pkPtr.get(), bagValStart, bagValLen,
                                               pwd, pwdLen,
                                               mbedtls_ctr_drbg_random,
                                               st.ctr_drbg);
                if(ret == 0)
                    *st.pkey = pkPtr.release();

                // else non-fatal: key import failure is skipped
            }
        }
        // else: unknown bag type, skip

        // advance past attributes
        p = const_cast<unsigned char*>(bagEnd);
    }

    return true;
}


// ---------------------------------------------------------------------------
// EncryptedData
// ---------------------------------------------------------------------------

static bool parseEncryptedData(const unsigned char* data, std::size_t len,
                               ParseState& st)
{
    unsigned char* p   = const_cast<unsigned char*>(data);
    const unsigned char* end = data + len;

    // EncryptedData ::= SEQUENCE { version INTEGER, encryptedContentInfo }
    std::size_t seqLen;
    if(mbedtls_asn1_get_tag(&p, end, &seqLen,
                             MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0)
        return false;
    const unsigned char* seqEnd = p + seqLen;

    // version INTEGER (skip)
    int version = 0;
    if(mbedtls_asn1_get_int(&p, seqEnd, &version) != 0)
        return false;

    // EncryptedContentInfo ::= SEQUENCE { contentType, algorithm, [0] encryptedContent }
    std::size_t eciLen;
    if(mbedtls_asn1_get_tag(&p, seqEnd, &eciLen,
                             MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0)
        return false;
    const unsigned char* eciEnd = p + eciLen;

    // contentType OID (skip)
    std::size_t ctLen;
    if(mbedtls_asn1_get_tag(&p, eciEnd, &ctLen, MBEDTLS_ASN1_OID) != 0)
        return false;
    p += ctLen;

    // contentEncryptionAlgorithm AlgorithmIdentifier
    mbedtls_asn1_buf algOid, algParams;
    if(mbedtls_asn1_get_alg(&p, eciEnd, &algOid, &algParams) != 0)
        return false;

    // [0] IMPLICIT encryptedContent (OCTET STRING tag replaced by context-specific)
    std::size_t encLen;
    if(mbedtls_asn1_get_tag(&p, eciEnd, &encLen,
                             MBEDTLS_ASN1_CONTEXT_SPECIFIC | 0) != 0)
        return false;
    const unsigned char* encData = p;

    // Decrypt
    std::vector<unsigned char> plain(encLen);
    std::size_t plainLen = 0;

    const unsigned char* pwd = reinterpret_cast<const unsigned char*>(st.passwd);
    std::size_t pwdLen = st.passwd ? std::strlen(st.passwd) : 0;

    if(OID_CMP(algOid, oidPbes2))
    {
        if(mbedtls_pkcs5_pbes2_ext(&algParams, MBEDTLS_PKCS5_DECRYPT,
                                    pwd, pwdLen,
                                    encData, encLen,
                                    plain.data(), plain.size(),
                                    &plainLen) != 0)
            return false;
    }
    else if(OID_CMP(algOid, oidPkcs12Pbe3DES))
    {
        mbedtls_asn1_buf p12params = algParams;
        if(mbedtls_pkcs12_pbe_ext(&p12params, MBEDTLS_PKCS12_PBE_DECRYPT,
                                   MBEDTLS_CIPHER_DES_EDE3_CBC,
                                   MBEDTLS_MD_SHA1,
                                   pwd, pwdLen,
                                   encData, encLen,
                                   plain.data(), plain.size(),
                                   &plainLen) != 0)
            return false;
    }
    else
    {
        return false; // unsupported encryption algorithm
    }

    return parseSafeContents(plain.data(), plainLen, st);
}


// ---------------------------------------------------------------------------
// ContentInfo
// ---------------------------------------------------------------------------

static bool parseContentInfo(unsigned char** p, const unsigned char* end,
                             ParseState& st)
{
    // ContentInfo ::= SEQUENCE { contentType OID, content [0] EXPLICIT ANY }
    std::size_t seqLen;
    if(mbedtls_asn1_get_tag(p, end, &seqLen,
                             MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0)
        return false;
    const unsigned char* seqEnd = *p + seqLen;

    // contentType OID
    mbedtls_asn1_buf ctOid;
    ctOid.tag = MBEDTLS_ASN1_OID;
    if(mbedtls_asn1_get_tag(p, seqEnd, &ctOid.len, MBEDTLS_ASN1_OID) != 0)
        return false;
    ctOid.p = *p;
    *p += ctOid.len;

    // content [0] EXPLICIT
    std::size_t ctxLen;
    if(mbedtls_asn1_get_tag(p, seqEnd, &ctxLen,
                             MBEDTLS_ASN1_CONTEXT_SPECIFIC |
                             MBEDTLS_ASN1_CONSTRUCTED | 0) != 0)
        return false;
    const unsigned char* ctxEnd = *p + ctxLen;
    unsigned char* ctxStart = *p;

    bool ok = true;

    if(OID_CMP(ctOid, oidPkcs7Data))
    {
        // pkcs7-data: [0] contains OCTET STRING wrapping SafeContents
        std::size_t osLen;
        if(mbedtls_asn1_get_tag(p, ctxEnd, &osLen,
                                 MBEDTLS_ASN1_OCTET_STRING) != 0)
            ok = false;
        else
            ok = parseSafeContents(*p, osLen, st);
    }
    else if(OID_CMP(ctOid, oidPkcs7EncData))
    {
        // pkcs7-encryptedData: [0] contains EncryptedData
        ok = parseEncryptedData(ctxStart, ctxLen, st);
    }
    // else: unknown content type - skip silently

    *p = const_cast<unsigned char*>(seqEnd);
    return ok;
}


// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

bool parsePkcs12(const unsigned char* data,
                 std::size_t          len,
                 const char*          passwd,
                 mbedtls_pk_context** pkey,
                 mbedtls_x509_crt**   cert,
                 mbedtls_x509_crt**   ca)
{
    *pkey = 0;
    *cert = 0;
    *ca   = 0;

    // Init RNG (needed by mbedtls_pk_parse_key for RSA blinding)
    mbedtls_entropy_context  entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    std::ostringstream seedBuilder;
    const Pt::int64_t ticks = Pt::System::Clock::getSystemTicks().toUSecs();
    seedBuilder << "Pt.Ssl.Pkcs12" << &ctr_drbg<< ticks;
    std::string seed = seedBuilder.str();
    const unsigned char* seedData = reinterpret_cast<const unsigned char*>( seed.data() );

    bool ok = (mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                      seedData, seed.size() ) == 0);

    if(ok)
    {
        ParseState st;
        st.pkey  = pkey;
        st.cert  = cert;
        st.ca    = ca;
        st.caTail = ca;
        st.passwd = passwd;
        st.ctr_drbg = &ctr_drbg;

        unsigned char* p   = const_cast<unsigned char*>(data);
        const unsigned char* end = data + len;

        // PFX ::= SEQUENCE { version INTEGER(3), authSafe ContentInfo, macData? }
        std::size_t pfxLen;
        if(mbedtls_asn1_get_tag(&p, end, &pfxLen,
                                 MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0)
        {
            ok = false;
        }
        else
        {
            const unsigned char* pfxEnd = p + pfxLen;

            // version INTEGER
            int version = 0;
            if(mbedtls_asn1_get_int(&p, pfxEnd, &version) != 0 || version != 3)
            {
                ok = false;
            }
            else
            {
                // authSafe ContentInfo: SEQUENCE { OID(pkcs7-data), [0] { OCTET STRING { AuthSafe } } }
                std::size_t ciLen;
                if(mbedtls_asn1_get_tag(&p, pfxEnd, &ciLen,
                                         MBEDTLS_ASN1_CONSTRUCTED |
                                         MBEDTLS_ASN1_SEQUENCE) != 0)
                {
                    ok = false;
                }
                else
                {
                    const unsigned char* ciEnd = p + ciLen;

                    // Skip OID
                    std::size_t oidLen;
                    if(mbedtls_asn1_get_tag(&p, ciEnd, &oidLen, MBEDTLS_ASN1_OID) != 0)
                    {
                        ok = false;
                    }
                    else
                    {
                        p += oidLen;

                        // [0] EXPLICIT
                        std::size_t ctxLen;
                        if(mbedtls_asn1_get_tag(&p, ciEnd, &ctxLen,
                                                 MBEDTLS_ASN1_CONTEXT_SPECIFIC |
                                                 MBEDTLS_ASN1_CONSTRUCTED | 0) != 0)
                        {
                            ok = false;
                        }
                        else
                        {
                            const unsigned char* ctxEnd = p + ctxLen;

                            // OCTET STRING containing AuthenticatedSafe DER
                            std::size_t osLen;
                            if(mbedtls_asn1_get_tag(&p, ctxEnd, &osLen,
                                                     MBEDTLS_ASN1_OCTET_STRING) != 0)
                            {
                                ok = false;
                            }
                            else
                            {
                                // AuthenticatedSafe ::= SEQUENCE OF ContentInfo
                                unsigned char* asP = p;
                                const unsigned char* asEnd = p + osLen;

                                std::size_t asLen;
                                if(mbedtls_asn1_get_tag(&asP, asEnd, &asLen,
                                                         MBEDTLS_ASN1_CONSTRUCTED |
                                                         MBEDTLS_ASN1_SEQUENCE) != 0)
                                {
                                    ok = false;
                                }
                                else
                                {
                                    const unsigned char* authEnd = asP + asLen;
                                    while(ok && asP < authEnd)
                                    {
                                        if( ! parseContentInfo(&asP, authEnd, st))
                                            ok = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    if( ! ok)
    {
        if(*pkey)
        {
            mbedtls_pk_free(*pkey);
            delete *pkey;
            *pkey = 0;
        }
        if(*cert)
        {
            mbedtls_x509_crt_free(*cert);
            delete *cert;
            *cert = 0;
        }

        while(*ca)
        {
            mbedtls_x509_crt* next = (*ca)->next;
            (*ca)->next = 0;
            mbedtls_x509_crt_free(*ca);
            delete *ca;
            *ca = next;
        }
        return false;
    }

    return true;
}

} // namespace Ssl

} // namespace Pt
