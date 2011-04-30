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
#ifndef PT_SSL_BASICCIPHER_H
#define PT_SSL_BASICCIPHER_H

#include <Pt/NonCopyable.h>
#include <Pt/Ssl/Api.h>

#include <iosfwd>
#include <string>

namespace Pt {
namespace Ssl {

//! \brief The base of all cipher classes.
//! Not all cipher will need password. Therefore we do not specify a method
//! to set a password in this class.
class PT_SSL_API BasicCipher : public NonCopyable {
    public:
        //! \brief Instantiate an empty basic-cipher object.
        BasicCipher(std::ostream& out);

        //! \brief Standard dtor.
        ~BasicCipher();

        //! \brief Set the output stream.
        void setOutputStream(std::ostream& out);

        //! \brief Add data (update the state of this cipher object).
        virtual void update(const char* str, int len) = 0;

        //! \brief Add data (update the state of this cipher object).
        virtual void update(const std::string& str) = 0;

        //! \brief Add data (update the state of this ciphert object).
        virtual void update(std::istream& is) = 0;

        //! \brief Finish a data encryption/decryption process.
        virtual void finish() = 0;

    protected:
        std::ostream* _out;
};

} // namespace Pt
} // namespace Ssl

#endif
