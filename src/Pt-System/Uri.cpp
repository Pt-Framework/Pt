/*
 * Copyright (C) 2010 Tommi Maekitalo
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

/*
3.1. Common Internet Scheme Syntax


   While the syntax for the rest of the URL may vary depending on the
   particular scheme selected, URL schemes that involve the direct use
   of an IP-based protocol to a specified host on the Internet use a
   common syntax for the scheme-specific data:

        //<user>:<password>@<host>:<port>/<url-path>

   Some or all of the parts "<user>:<password>@", ":<password>",
   ":<port>", and "/<url-path>" may be excluded.  The scheme specific
   data start with a double slash "//" to indicate that it complies with
   the common Internet scheme syntax. The different components obey the
   following rules:

    user
        An optional user name. Some schemes (e.g., ftp) allow the
        specification of a user name.

    password
        An optional password. If present, it follows the user
        name separated from it by a colon.

   The user name (and password), if present, are followed by a
   commercial at-sign "@". Within the user and password field, any ":",
   "@", or "/" must be encoded.

   Note that an empty user name or password is different than no user
   name or password; there is no way to specify a password without
   specifying a user name. E.g., <URL:ftp://@host.com/> has an empty
   user name and no password, <URL:ftp://host.com/> has no user name,
   while <URL:ftp://foo:@host.com/> has a user name of "foo" and an
   empty password.

    host
        The fully qualified domain name of a network host, or its IP
        address as a set of four decimal digit groups separated by
        ".". Fully qualified domain names take the form as described
        in Section 3.5 of RFC 1034 [13] and Section 2.1 of RFC 1123
        [5]: a sequence of domain labels separated by ".", each domain
        label starting and ending with an alphanumerical character and
        possibly also containing "-" characters. The rightmost domain
        label will never start with a digit, though, which
        syntactically distinguishes all domain names from the IP
        addresses.

    port
        The port number to connect to. Most schemes designate
        protocols that have a default port number. Another port number
        may optionally be supplied, in decimal, separated from the
        host by a colon. If the port is omitted, the colon is as well.

    url-path
        The rest of the locator consists of data specific to the
        scheme, and is known as the "url-path". It supplies the
        details of how the specified resource can be accessed. Note
        that the "/" between the host (or port) and the url-path is
        NOT part of the url-path.
*/

#include <Pt/System/Uri.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cctype>

namespace {

void throwInvalid(const std::string& uri)
{
  throw Pt::System::InvalidUri(uri);
}

} // namespace

namespace Pt {

namespace System {

InvalidUri::InvalidUri(const std::string& uri)
: std::runtime_error("invalid URI")
, _uri(uri)
{
}


Uri::Uri(const std::string& uri)
: _port(0)
, _ipv6(false)
{
    enum {
      state_0,
      state_protocol,
      state_postprotocol,
      state_postprotocol2,
      state_postprotocol3,
      state_user_or_host,
      state_password_or_port,
      state_password,
      state_host,
      state_ipv6,
      state_ipv6ok,
      state_ipv6end,
      state_port,
      state_path,
      state_query,
      state_fragment
    } state = state_0;

    std::string token;
    bool hasPort = false;

    for(std::string::const_iterator it = uri.begin(); it != uri.end(); ++it)
    {
        char ch = *it;
        switch (state)
        {
            case state_0:
              if (std::isalpha(ch))
              {
                _protocol = ch;
                state = state_protocol;
              }
              else if (!std::isspace(ch))
                throwInvalid(uri);
              break;

            case state_protocol:
              if (std::isalpha(ch))
                _protocol += ch;
              else if (ch == ':')
                state = state_postprotocol;
              else
                throwInvalid(uri);
              break;

            case state_postprotocol:
              if (ch == '/')
                state = state_postprotocol2;
              else
                throwInvalid(uri);
              break;

            case state_postprotocol2:
              if (ch == '/')
                state = state_postprotocol3;
              else
                throwInvalid(uri);
              break;

            case state_postprotocol3:
              if (ch == '[')
              {
                _ipv6 = true;
                state = state_ipv6;
              }
              else if (ch == '/')
              {
                if(_protocol != "file") // TODO: the third slash is not part of the url-path
                  _path = ch;
                state = state_path;
              }
              else
              {
                _user = ch;
                state = state_user_or_host;
              }
              break;

            case state_user_or_host:
              if (ch == ':')
                state = state_password_or_port;
              else if (ch == '/')
              {
                _host = _user;
                _user.clear();
                _path = ch;
                state = state_path;
              }
              else if (ch == '@')
                state = state_host;
              else
                _user += ch;
              break;

            case state_password_or_port:
              if (ch == '@')
              {
                _port = 0;
                state = state_host;
              }
              else if (ch == '/')
              {
                _host = _user;
                _user.clear();
                _password.clear();
                _path = ch;
                state = state_path;
              }
              else if (std::isdigit(ch))
              {
                hasPort = true;
                _password += ch;
                _port = _port * 10 + ch - '0';
              }
              else
              {
                _port = 0;
                hasPort = false;
                _password += ch;
                state = state_password;
              }
              break;

            case state_password:
              if (ch == '@')
                state = state_host;
              else
                _password += ch;
              break;

            case state_host:
              if (ch == '/')
              {
                _path = ch;
                state = state_path;
              }
              else if (ch == ':')
                state = state_port;
              else if (_host.empty() && ch == '[')
              {
                _ipv6 = true;
                state = state_ipv6;
              }
              else
                _host += ch;
              break;

            case state_ipv6:
              if (ch == ':')
              {
                _host += ch;
                state = state_ipv6ok;
              }
              else if (std::isdigit(ch)
                    || (ch >= 'a' && ch <= 'f')
                    || (ch >= 'F' && ch <= 'F'))
                _host += ch;
              else
                throwInvalid(uri);
              break;

            case state_ipv6ok:
              if (ch == ']')
                state = state_ipv6end;
              else if (std::isdigit(ch)
                    || (ch >= 'a' && ch <= 'f')
                    || (ch >= 'F' && ch <= 'F')
                    || ch == ':')
                _host += ch;
              else
                throwInvalid(uri);
              break;

            case state_ipv6end:
              if (ch == ':')
              {
                hasPort = true;
                state = state_port;
              }
              else if (ch == '/')
              {
                _path = ch;
                state = state_path;
              }
              else
                throwInvalid(uri);
              break;

            case state_port:
              if (ch == '/')
              {
                _path = ch;
                state = state_path;
              }
              else if (std::isdigit(ch))
              {
                hasPort = true;
                _port = _port * 10 + ch - '0';
              }
              else
                throwInvalid(uri);
              break;

            case state_path:
              if (ch == '?')
                state = state_query;
              else if (ch == '#')
                state = state_fragment;
              else
                _path += ch;
              break;

            case state_query:
              if (ch == '#')
                state = state_fragment;
              else
                _query += ch;
              break;

            case state_fragment:
              _fragment += ch;
              break;
        }
    }

    switch (state)
    {
        case state_port:
        case state_host:
        case state_path:
        case state_query:
        case state_fragment:
          break;

        case state_user_or_host:
          _host = _user;
          _user.clear();
          break;

        default:
          throwInvalid(uri);
    }

    if (!hasPort)
    {
        if (_protocol == "http")
          _port = 80;
        else if (_protocol == "https")
          _port = 443;
        else if (_protocol == "ftp")
          _port = 21;
    }
}

std::string Uri::str() const
{
    std::ostringstream s;
    s << _protocol << "://";
    if (!_user.empty() || !_password.empty())
    {
        s << _user;
        if (!_password.empty())
          s << ':' << _password;
        s << '@';
    }

    if (_ipv6)
        s << '[' << _host << ']';
    else
        s << _host;

    if (!(_port == 0
       || (_protocol == "http"  && _port == 80)
       || (_protocol == "https" && _port == 443)
       || (_protocol == "ftp"   && _port == 21)))
      s << ':' << _port;

    s << _path;
    
    if (!_query.empty())
       s << '?' << _query;
    
    if (!_fragment.empty())
        s << '#' << _fragment;

    return s.str();
}

} // namespace System

} // namespace Pt
