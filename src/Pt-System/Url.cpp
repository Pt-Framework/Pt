/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
 *   Copyright (C) 2005 Aloysius Indrayanto                                *
 *                                                                         *
 *   This code is based on code written by Christian Prochnow              *
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
#include "Pt/System/Url.h"
#include <sstream>
#include <iomanip>
#include <cstdlib> 

namespace Pt {

namespace System {

InvalidUrl::InvalidUrl(const char* _what, const SourceInfo& _si)
: std::logic_error(_what + _si)
{ }


Url::Url()
: _proto("file")
, _port(0)
, _path("/")
{
}


Url::Url(const std::string& url)
{
    *this = url;
}


Url::Url(const Url& url)
: _proto(url._proto), _host(url._host), _user(url._user),
  _passwd(url._passwd), _port(url._port), _path(url._path),
  _args(url._args), _anchor(url._anchor)
{
}


void Url::clear() {
    _proto.clear();
    _host.clear();
    _user.clear();
    _passwd.clear();
    _port = 0;
    _path.clear();
    _args.clear();
    _anchor.clear();
}


void Url::setPath(const std::string& path)
{
    if( path.empty() )
    {
        _path = "/";
    }
    else
    {
        // make sure we have a leading slash in the path ...
        if(path[0] != '/')
            _path = "/" + path;
        else
            _path = path;
    }
}


Url::ArgumentMap Url::fromString(const std::string& args)
{
    ArgumentMap pargs;
    std::istringstream is(args);

    char ch;
    bool invalue = false;
    std::ostringstream os_name, os_val;

    do
    {
        is >> ch;

        if(ch == '?' || ch == '&' || !is)
        {
            std::string name = os_name.str();
            std::string value = os_val.str();

            if(!name.empty())
                pargs[decode(name)] = decode(value);

            invalue = false;
            os_name.str("");
            os_val.str("");
            continue;
        }

        if(ch == '=')
        {
            invalue = true;
            continue;
        }

        if(invalue)
            os_val << ch;
        else
            os_name << ch;

    } while(is);

    return pargs;
}


std::string Url::toString(const ArgumentMap& args)
{
    std::ostringstream os;

    // add Url arguments ...
    Url::ArgumentMap::const_iterator i = args.begin();
    while(i != args.end())
    {
        if(i != args.begin())
            os << '&';

        // add urlencoded arg...
        os << Url::encode(i->first)
           << '='
           << Url::encode(i->second);

        ++i;
    }

    return os.str();
}


void Url::addArg(const std::string& name, const std::string& value)
{
    _args[name] = value;
}


const std::string& Url::arg(const std::string& name) const
{
    static std::string emptyStr;
    ArgumentMap::const_iterator i = _args.find(name);
    if(i != _args.end())
        return i->second;

    return emptyStr;
}


void Url::setArgs(const ArgumentMap& args)
{
    _args = args;
}


void Url::setArgs(const std::string& args)
{
    _args = fromString(args);
}


const Url::ArgumentMap& Url::args() const
{
    return _args;
}


std::string Url::args(std::string /*<- dummy */) const
{
    return toString(_args);
}


void Url::clearArgs()
{
    _args.clear();
}


Url& Url::operator=(const Url& url)
{
    _proto  = url._proto;
    _host   = url._host;
    _user   = url._user;
    _passwd = url._passwd;
    _port   = url._port;
    _path   = url._path;
    _args   = url._args;
    _anchor = url._anchor;

    return *this;
}


Url& Url::operator=(const char* url)
{
    return this->operator=( std::string(url) );
}


Url& Url::operator=(const std::string& url)
{
    using std::string;
    // file-Urls are handled specially ...
    if(url.substr(0, 5) == "file:" || url.substr(0, 1) == "/")
    {
        _proto  = "file";
        _host   = "";
        _user   = "";
        _passwd = "";
        _port   = 0;

        if(url.find("file:") != string::npos) {
            _path = decode( url.substr(6, string::npos) );
        }
        else
            _path = decode( url );

        _args.clear();
        _anchor = "";

        return *this;
    }

    // search for protocol delimiter ...
    string::size_type protoEndPos = url.find("://");
    if(protoEndPos == string::npos) {
        throw InvalidUrl("Invalid url", PT_SOURCEINFO);
    }
    string proto = url.substr(0, protoEndPos);

    // get the url-component starting positions ...
    string::size_type hostStartPos = protoEndPos + 3;
    string::size_type pathStartPos = url.find('/', hostStartPos);
    string::size_type argsStartPos = url.find('?', pathStartPos == string::npos ? hostStartPos : pathStartPos);
    string::size_type anchorStartPos = url.find('#', argsStartPos == string::npos ? hostStartPos : argsStartPos);

    // find the lenght of the hostname ...
    string::size_type hostLen;
    if(pathStartPos != string::npos)
        hostLen = pathStartPos - hostStartPos;
    else if(argsStartPos != string::npos)
        hostLen = argsStartPos - hostStartPos;
    else if(anchorStartPos != string::npos)
        hostLen = anchorStartPos - hostStartPos;
    else
        hostLen = string::npos;

    // get the hostname (including username/passwd and port)...
    string host = url.substr(hostStartPos, hostLen);

    // find the length of the path ...
    string::size_type pathLen;
    if(argsStartPos != string::npos)
        pathLen = argsStartPos - pathStartPos;
    else if(anchorStartPos != string::npos)
        pathLen = anchorStartPos - pathStartPos;
    else
        pathLen = string::npos;

    // get the path ...
    string path = "/";
    if(pathStartPos != string::npos)
        path = url.substr(pathStartPos, pathLen);

    string::size_type argsLen;
    if(anchorStartPos != string::npos)
        argsLen = anchorStartPos - argsStartPos;
    else
        argsLen = string::npos;

    // get the arguments ...
    string args;
    if(argsStartPos != string::npos)
        args = url.substr(argsStartPos, argsLen);

    // get the anchor ...
    string anchor;
    if(anchorStartPos != string::npos)
        anchor = url.substr(anchorStartPos + 1, string::npos);

    // get username and password from hostname ...
    string userPasswd, user, passwd;
    hostStartPos = host.find("@");
    if(hostStartPos != string::npos)
    {
        userPasswd = host.substr(0, hostStartPos);
        host = host.substr(hostStartPos + 1, string::npos);

        string::size_type passwdStartPos = userPasswd.find(":");
        if(passwdStartPos != string::npos)
        {
            user   = userPasswd.substr(0, passwdStartPos);
            passwd = userPasswd.substr(passwdStartPos + 1, string::npos);
        }
    }

    // get port from hostname ...
    unsigned short port = 0;
    if(proto == "http")
        port = 80;

    string::size_type portStartPos = host.find(":");
    if(portStartPos != string::npos)
    {
        string portStr = host.substr(portStartPos + 1, string::npos);
        host = host.substr(0, portStartPos);
        port = std::atoi(portStr.c_str());
    }

    _proto  = proto;
    _host   = host;
    _user   = user;
    _passwd = passwd;
    _port   = port;
    _path   = path;
    _args   = fromString(args);
    _anchor = anchor;

    return *this;
}


bool Url::operator==(const Url& url) const
{
    if(_proto == url._proto && _host   == url._host   &&
       _user  == url._user  && _passwd == url._passwd &&
       _port  == url._port  && _path   == url._path   &&
       _args  == url._args  && _anchor == url._anchor)
        return true;

    return false;
}


std::string Url::str() const
{
    std::ostringstream os;
    os << *this;
    return os.str();
}


std::ostream& operator<<(std::ostream& os, const Url& url)
{
    // add protocol...
    if(url.protocol() == "file") {
        os << url._proto << ":/" << url.path();
        return os;
    }

    os << url._proto << "://";

    // add username ...
    if(!url._user.empty())
    {
        os << url._user;
        os << '@';
    }

    // add hostname
    os << url._host;

    // add port ...
    if(url._port)
        os << ':' << url._port;

    // add path ...
    os << url._path;

    // add Url arguments ...
    std::string args = Url::toString(url._args);
    if(!args.empty())
        os << args;

    // add anchor if set ...
    if(!url._anchor.empty()) {
        os << "#" << url._anchor;
    }

    return os;
}

std::istream& operator>>(std::istream& is, Url& url)
{
    std::string str;
    if(is)
    {
        is >> str;
        url = str;
    }

    return is;
}


static bool charNeedEncode(unsigned char ch)
{
    bool ret = false;

    if((ch <= 0x1f) || (ch == 0x7f) ||    // ASCII control characters...
        (ch >= 0x80) ||                    // non-ASCII characters ...
        (ch == 0x24) || (ch == 0x26) ||    // reserved characters...
        (ch == 0x2b) || (ch == 0x2c) ||
        (ch == 0x2f) || (ch == 0x3a) ||
        (ch == 0x3b) || (ch == 0x3d) ||
        (ch == 0x3f) || (ch == 0x40) ||
        (ch == 0x20) || (ch == 0x22) ||    // unsafe characters ...
        (ch == 0x3c) || (ch == 0x3e) ||
        (ch == 0x23) || (ch == 0x25) ||
        (ch == 0x7b) || (ch == 0x7d) ||
        (ch == 0x7c) || (ch == 0x5c) ||
        (ch == 0x5e) || (ch == 0x7e) ||
        (ch == 0x5b) || (ch == 0x5d) ||
        (ch == 0x60))
        ret = true;

    return ret;
}


std::string Url::encode(const std::string& str)
{
    std::ostringstream os;
    os << std::setfill('0');

    for(std::string::size_type i = 0; i < str.size(); ++i)
    {
        char ch = str[i];
        if(charNeedEncode(ch))
            os << '%' << std::setw(2) << std::hex << (((int)ch) & 0x000000ff);
        else
            os << ch;
    }

    return os.str();
}

std::string Url::decode(const std::string& str)
{
    std::ostringstream os;
    std::istringstream is(str);

    char ch;
    while((is >> ch))
    {
        if(ch == '%')
        {
            int val;
            is >> std::hex >> val;
            os << (char) val;
        }
        else
        {
            os << ch;
        }
    }

    return os.str();
}

} // namespace System

} // !namespace P
