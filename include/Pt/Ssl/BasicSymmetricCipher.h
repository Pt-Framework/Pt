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
#ifndef PT_SSL_BASICSYMETRICCIPHER_H
#define PT_SSL_BASICSYMETRICCIPHER_H

#include <Pt/Ssl/BasicCipher.h>
#include <Pt/Ssl/Exception.h>

#include <vector>

namespace Pt {
namespace Ssl {

//! \brief The base of all symmetric-cipher classes.
//! Symmetric-cipher needs password. Therefore we specify a method
//! to set a password in this class.
//!
//! NOTE: We can later use enum to specify the cipher or derive this class
class PT_SSL_API BasicSymmetricCipher : public BasicCipher {
    public:
        //! \brief Instantiate an empty symmetric-cipher object.
        BasicSymmetricCipher(std::iostream& ios);

        //! \brief Standard dtor.
        virtual ~BasicSymmetricCipher();

        //! \brief Start a data encryption process.
        virtual void startEncrypt(const std::string& password);

        //! \brief Start a data decryption process.
        virtual void startDecrypt(const std::string& password);

    protected:
        enum Mode {
            Invalid,
            Encrypt,
            Decrypt
        };

    protected:
        virtual int sync();
        virtual int_type underflow();
        virtual int_type overflow(int_type ch);
        
    protected:
        Mode              _mode;
        std::string       _pswd;
        std::vector<char> _ioBuf;  // Input/output buffer
        std::vector<char> _cnvBuf; // Conversion buffer
        
};

} // namespace Pt
} // namespace Ssl

#endif
