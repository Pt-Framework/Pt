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
#ifndef PT_SSL_SECUREDIGEST_H
#define PT_SSL_SECUREDIGEST_H

#include <Pt/NonCopyable.h>
#include <Pt/Ssl/SSLPublicKey.h>
#include <Pt/Ssl/SSLPrivateKey.h>

namespace Pt {
namespace Ssl {

//! \brief Secure digest.
class PT_SSL_API SecureDigest : public NonCopyable, public std::streambuf {
    public:
        //! \brief Digest type.
        enum DigestType {
            MD5_Digest, //!< Use MD5 digest
            SHA1_Digest //!< Use SHA1 digest (recommended)
        };
        
    public:
        //! \brief Instantiate an empty secure-digest object.
        SecureDigest();

        //! \brief Instantiate a secure-digest object for data signing.
        SecureDigest(const SSLPrivateKey& pkey, DigestType digestType = SHA1_Digest);

        //! \brief Instantiate a secure-digest object for data verification.
        SecureDigest(const SSLPublicKey& pkey, const std::string& sig, DigestType digestType = SHA1_Digest);

        //! \brief Standard dtor.
        ~SecureDigest();
        
        //! \brief Start a data signing process.
        void start(const SSLPrivateKey& pkey, DigestType digestType = SHA1_Digest);

        //! \brief Start a data verification process.
        void start(const SSLPublicKey& pkey, const std::string& sig, DigestType digestType = SHA1_Digest);
       
        //! \brief Add data (update the state of this secure-digest object).
        void update(const char* str, int len);

        //! \brief Add data (update the state of this secure-digest object).
        void update(const std::string& str);

        //! \brief Add data (update the state of this secure-digest object).
        void update(std::istream& is);

        //! \brief Finish a data signing/verification process.
        //! This function will return true if the operation was a success. In the case
        //! of data verification, true means that the signature is a match.
        bool finish();

        //! \brief Get the signature.
        inline const std::string& getSignature() const
        { return _sig; }

    protected:
        virtual int sync();
        virtual int_type underflow();
        virtual int_type overflow(int_type ch);

    private:
        // State
        std::string       _sig;
        size_t            _rawSigSize;
        bio_st*           _sbio;
        env_md_ctx_st*    _mctx;
        std::vector<char> _inpBuf;

        // Add data (update the state of this secure-digest object).
        void doUpdate();

        // Convert digest enum to string
        static const char* digestEnumToString(DigestType digestType);
};

} // namespace Ssl
} // namespace Pt

#endif
