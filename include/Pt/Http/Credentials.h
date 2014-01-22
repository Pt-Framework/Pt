/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Credentials_h
#define Pt_Http_Credentials_h

#include <Pt/Http/Api.h>
#include <string>
#include <map>

namespace Pt {

namespace Http {

/** @brief Credentials for authorization and authentication.
*/
class Credential
{
    public:
        /** @brief Default Constructor.
        */
        Credential()
        {}

        /** @brief Construct with user and password.
        */
        Credential(const std::string& user, const std::string& passwd)
        : _user(user)
        , _passwd(passwd)
        {}

        /** @brief Construct with user and password.
        */
        Credential(const char* user, const char* passwd)
        : _user(user)
        , _passwd(passwd)
        {}

        /** @brief Set user and password.
        */
        void set(const std::string& user, const std::string& passwd)
        {
            _user = user;
            _passwd = passwd;
        }

        /** @brief Set user and password.
        */
        void set(const char* user, const char* passwd)
        {
            _user = user;
            _passwd = passwd;
        }

        /** @brief Returns the user name.
        */
        std::string& user()
        { return _user; }
        
        /** @brief Returns the user name.
        */
        const std::string& user() const
        { return _user; }

        /** @brief Returns the password.
        */
        std::string& password()
        { return _passwd; }
        
        /** @brief Returns the password.
        */
        const std::string& password() const
        { return _passwd; }

    private:
        std::string _user;
        std::string _passwd;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Credentials_h
