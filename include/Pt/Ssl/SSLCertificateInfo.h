/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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
#ifndef PT_SSL_SSLCERTIFICATEINFO_H
#define PT_SSL_SSLCERTIFICATEINFO_H

//#undef NLOG

#include <Pt/Ssl/Exception.h>

// Forward declaration of some OpenSSL structures
struct ssl_ctx_st;
struct ssl_st;
struct bio_st;
struct x509_st;
struct evp_pkey_st;

namespace Pt {
namespace Ssl {

//! \brief Certificate information.
class PT_SSL_API SSLCertificateInfo {
    public:
        long        version;
        long        serialNumber;
        std::string issuerName;
        std::string issuerNameHash;
        std::string subjectName;
        std::string subjectNameHash;
        std::string notBefore;
        std::string notAfter;
        std::string fingerprintType;
        std::string fingerprintHash;

        friend class SSLCertificateChain;
        friend class SSLTrustedCertificate;

    private:
        inline SSLCertificateInfo()
        : version     (0),
          serialNumber(0)
        {}

        inline SSLCertificateInfo(long               version_,
                                  long               serialNumber_,
                                  const std::string& issuerName_,
                                  const std::string& issuerNameHash_,
                                  const std::string& subjectName_,
                                  const std::string& subjectNameHash_,
                                  const std::string& notBefore_,
                                  const std::string& notAfter_,
                                  const std::string& fingerprintType_,
                                  const std::string& fingerprintHash_)
        : version        (version_        ),
          serialNumber   (serialNumber_   ),
          issuerName     (issuerName_     ),
          issuerNameHash (issuerNameHash_ ),
          subjectName    (subjectName_    ),
          subjectNameHash(subjectNameHash_),
          notBefore      (notBefore_      ),
          notAfter       (notAfter_       ),
          fingerprintType(fingerprintType_),
          fingerprintHash(fingerprintHash_)
        {}

        inline void set(long               version_,
                        long               serialNumber_,
                        const std::string& issuerName_,
                        const std::string& issuerNameHash_,
                        const std::string& subjectName_,
                        const std::string& subjectNameHash_,
                        const std::string& notBefore_,
                        const std::string& notAfter_,
                        const std::string& fingerprintType_,
                        const std::string& fingerprintHash_)
        {
            version         = version_;
            serialNumber    = serialNumber_;
            issuerName      = issuerName_;
            issuerNameHash  = issuerNameHash_;
            subjectName     = subjectName_;
            subjectNameHash = subjectNameHash_;
            notBefore       = notBefore_;
            notAfter        = notAfter_;
            fingerprintType = fingerprintType_;
            fingerprintHash = fingerprintHash_;
        }

        inline void clear()
        {
            version         = 0;
            serialNumber    = 0;
            issuerName      = "";
            issuerNameHash  = "";
            subjectName     = "";
            subjectNameHash = "";
            notBefore       = "";
            notAfter        = "";
            fingerprintType = "";
            fingerprintHash = "";
        }
};

} // namespace Pt
} // namespace Ssl

#endif
