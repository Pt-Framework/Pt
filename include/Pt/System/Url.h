/*
 * Copyright (C) 2004 Marc Boris Duerner
 * Copyright (C) 2005 Aloysius Indrayanto
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
#ifndef Pt_System_Url_h
#define Pt_System_Url_h

#include <Pt/System/Api.h>
#include <Pt/SourceInfo.h>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>

namespace Pt {

namespace System {

class PT_SYSTEM_API InvalidUrl : public std::invalid_argument
{
    public:
        InvalidUrl(const char* msg);

        InvalidUrl(const std::string& msg);

        ~InvalidUrl() throw()
        {}
};


//! Uniform resource locator
/*!
    This class is used to parse and store uniform resource
    locators. When storing a Url with password the password
    will not be printed out by the operator<< but can
    be retrieved via password().
*/
class PT_SYSTEM_API Url {
    public:
        typedef std::map<std::string, std::string> ArgumentMap;

        //! Default constructor
        /*!
          Initializes the Url to 'file:///'
        */
        Url();

        //! Parse Url constructor
        //Url(const String& url) throw(InvalidUrl);

        //! Parse Url constructor
        Url(const std::string& url);

        //! Copy constructor
        Url(const Url& url);

        ~Url()
        {}

        void clear();

        const std::string& protocol() const
        { return _proto; }

        void setProtocol(const std::string& proto)
        { _proto = proto; }

        const std::string& host() const
        { return _host; }

        void setHost(const std::string& host)
        { _host = host; }

        const std::string& user() const
        { return _user; }

        void setUser(const std::string& user)
        { _user = user; }

        const std::string& password() const
        { return _passwd; }

        void setPassword(const std::string& passwd)
        { _passwd = passwd; }

        unsigned short port() const
        { return _port; }

        void setPort(unsigned short port)
        { _port = port; }

        const std::string& path() const
        { return _path; }

        void setPath(const std::string& path);

        //! Returns the argument for the given key
        std::string& arg(const std::string& key);

        //! Add argument to Url
        void addArg(const std::string& name, const std::string& value);

        //! Set arguments
        void setArgs(const ArgumentMap& args);

        //! Set arguments from Url-encoded string
        void setArgs(const std::string& str);

        //! Returns arguments
        const ArgumentMap& args() const;

        //! Returns arguments as url encoded string
        std::string args(std::string) const;

        //! Clear arguments
        void clearArgs();

        //! Returns the anchor
        const std::string& anchor() const
        { return _anchor; }

        //! Sets the anchor
        void setAnchor(const std::string& anch)
        { _anchor = anch; }

        //! Url assignment operator
        Url& operator=(const Url& url);

        //! String assignment operator
        //Url& Url::operator=(const String& url) throw(InvalidUrl);

        //! String assignment operator
        Url& operator=(const std::string& url);

        //! Classic C-style string assignment operator
        Url& operator=(const char* url);

        //! Compare Urls
        bool operator==(const Url& url) const;

        //! Returns the Url as non Url-encoded string
        std::string str() const;

        //! Url-encodes the given string
        static std::string encode(const std::string& str);

        //! Url-decodes the given string
        static std::string decode(const std::string& str);

        friend std::ostream& operator<<(std::ostream& os,
            const Url& url);

        friend std::istream& operator>>(std::istream& is, Url& url);

        //! Returns a ArgumentMap for given Url-encoded argument-string
        static ArgumentMap fromString(const std::string& args);

        //! Returns the Url-encoded string representation of the given ArgumentMap
        static std::string toString(const ArgumentMap& args);

    private:
        std::string    _proto;
        std::string    _host;
        std::string    _user;
        std::string    _passwd;
        unsigned short _port;
        std::string    _path;
        ArgumentMap    _args;
        std::string    _anchor;
};

} // namespace System

} // !namespace Pt

#endif
