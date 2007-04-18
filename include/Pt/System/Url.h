/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_System_Url_h
#define Pt_System_Url_h

#include <Pt/System/Api.h>
#include <Pt/Exception.h>
#include <iostream>
#include <map>
#include <string>

namespace Pt {
namespace System {

class PT_SYSTEM_API InvalidUrl : public std::logic_error
{
public:
    inline InvalidUrl(const char* _what, const SourceInfo& _si)
    : std::logic_error(_what + _si)
    { }

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

        //! Parse Url constructor
        Url(const char* url);

        //! Copy constructor
        Url(const Url& url);

        ~Url();

        void clear();

        const std::string& protocol() const;
        void setProtocol(const std::string& proto);

        const std::string& host() const;
        void setHost(const std::string& host);

        const std::string& user() const;
        void setUser(const std::string& user);

        const std::string& password() const;
        void setPassword(const std::string& passwd);

        unsigned short port() const;
        void setPort(unsigned short port);

        const std::string& path() const;
        void setPath(const std::string& path);

        //! Returns the argument for the given key
        const std::string& arg(const std::string& key) const;

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
        const std::string& anchor() const;

        //! Sets the anchor
        void setAnchor(const std::string& anch);

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
