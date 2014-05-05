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
#ifndef PT_SSL_TRIPLEDESCIPHER_H
#define PT_SSL_TRIPLEDESCIPHER_H

#include <Pt/Ssl/BasicSymmetricCipher.h>

namespace Pt {
namespace Ssl {

//! \brief Triple-DES (Triple Data Encryption Standard) symmetric-block-cipher classes.
class PT_SSL_API TripleDESCipher  : public BasicSymmetricCipher {
    public:
        using BasicSymmetricCipher::SaltType;
        using BasicSymmetricCipher::Mode;
        
    public:
        //! \brief Instantiate an empty symmetric-block-cipher object.
        TripleDESCipher(std::iostream& ios, Mode mode = CBC);

        //! \brief Standard dtor.
        virtual ~TripleDESCipher();

        //! \brief Set the mode of operation of the cipher.
        virtual void setMode(Mode mode = CBC);
        
    protected:
        virtual const char* getOpenSSLCipherName() const;

    private:
        Mode _mode;
};

} // namespace Pt
} // namespace Ssl

#endif
