/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include <Pt/Net/HttpParser.h>
#include <iostream>
#include <cctype>
#include <algorithm>

#define log_debug(e)
#define log_warn(e)

namespace Pt {

namespace Net {

    namespace
    {
        std::string chartoprint(char ch)
        {
            const static char hex[] = "0123456789abcdef";
            if (std::isprint(ch))
                return std::string(1, '\'') + ch + '\'';
            else
                return std::string("'\\x") + hex[(ch >> 4) & 0xf] + hex[ch & 0xf] + '\'';
        }

        inline bool istokenchar(char ch)
        {
            static const char s[] = "\"(),/:;<=>?@[\\]{}";
            return std::isalpha(ch) || std::binary_search(s, s + sizeof(s) - 1, ch);
        }

        inline bool isHexDigit(char ch)
        {
            return (ch >= '0' && ch <= '9')
                || (ch >= 'A' && ch <= 'Z')
                || (ch >= 'a' && ch <= 'z');
        }

        inline unsigned valueOfHexDigit(char ch)
        {
            return ch >= '0' && ch <= '9' ? ch - '0'
                 : ch >= 'a' && ch <= 'z' ? ch - 'a' + 10
                 : ch >= 'A' && ch <= 'Z' ? ch - 'A' + 10
                 : 0;
        }
    }

    void HttpRequestParser::Event::onMethod(const std::string& method)
    {
    }

    void HttpRequestParser::Event::onUrl(const std::string& url)
    {
    }

    void HttpRequestParser::Event::onUrlParam(const std::string& q)
    {
    }

    void HttpRequestParser::Event::onHttpVersion(unsigned major, unsigned minor)
    {
    }

    void HttpRequestParser::Event::onKey(const std::string& key)
    {
    }

    void HttpRequestParser::Event::onValue(const std::string& value)
    {
    }

    void HttpRequestParser::Event::onEnd()
    {
    }

    std::size_t HttpRequestParser::advance(std::istream& is)
    {
        std::size_t ret = 0;

        std::streambuf* sb = is.rdbuf();

        while (sb->in_avail() > 0)
        {
            ++ret;
            if (parse(sb->sbumpc()))
                return ret;
        }

        return ret;
    }

    void HttpRequestParser::state_cmd0(char ch)
    {
        if (istokenchar(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HttpRequestParser::state_cmd;
            return;
        }
        else if (ch != ' ' && ch != '\t')
        {
            log_warn("invalid character " << chartoprint(ch) << " in method");
            state = &HttpRequestParser::state_error;
            return;
        }
        else
        {
            state = &HttpRequestParser::state_cmd;
            return;
        }
    }

    void HttpRequestParser::state_cmd(char ch)
    {
        if (istokenchar(ch))
        {
            token += ch;
            return;
        }
        else if (ch == ' ')
        {
            log_debug("method=" << token);
            ev.onMethod(token);
            state = &HttpRequestParser::state_url0;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in method");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_url0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch > ' ')
        {
            token.reserve(32);
            token = ch;
            state = &HttpRequestParser::state_url;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_url(char ch)
    {
        if (ch == '?')
        {
            log_debug("url=" << token);
            ev.onUrl(token);
            token.clear();
            token.reserve(32);
            state = &HttpRequestParser::state_qparam;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            log_debug("url=" << token);
            ev.onUrl(token);
            token.clear();
            token.reserve(32);
            state = &HttpRequestParser::state_protocol0;
            return;
        }
        else if (ch == '+')
        {
            token += ' ';
            return;
        }
        else if (ch == '%')
        {
            token += ch;
            state = &HttpRequestParser::state_urlesc;
            return;
        }
        else if (ch > ' ')
        {
            token += ch;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in url");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_urlesc(char ch)
    {
        if (isHexDigit(ch))
        {
            if (token.size() >= 2 && token[token.size() - 2] == '%')
            {
                unsigned v = (valueOfHexDigit(token[token.size() - 1]) << 4) | valueOfHexDigit(ch);
                token[token.size() - 2] = static_cast<char>(v);
                token.resize(token.size() - 1);
                state = &HttpRequestParser::state_url;
                return;
            }
            else
            {
                token += ch;
                return;
            }
        }
        else
        {
            log_warn("invalid hex digit " << chartoprint(ch) << " in url");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_qparam(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            log_debug("queryString=" << token);
            ev.onUrlParam(token);
            token.clear();
            token.reserve(32);
            state = &HttpRequestParser::state_protocol0;
            return;
        }
        else
        {
            token += ch;
            return;
        }
    }

    void HttpRequestParser::state_protocol0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (std::isalpha(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HttpRequestParser::state_protocol;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http protocol field");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_protocol(char ch)
    {
        if (ch == ' ' || ch == '\t' || ch == '/')
        {
            if (token != "HTTP")
            {
                log_warn("invalid protocol " << token << " in http protocol field");
                state = &HttpRequestParser::state_error;
                return;
            }
            else
            {
                state = (ch == '/' ? &HttpRequestParser::state_version_major : &HttpRequestParser::state_version0);
                return;
            }
        }
        else if (std::isalpha(ch))
        {
            token += std::toupper(ch);
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http protocol field");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_version0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '/')
        {
            state = &HttpRequestParser::state_version_major;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_version_major(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '1')
        {
            state = &HttpRequestParser::state_version_major_e;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_version_major_e(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            state = &HttpRequestParser::state_version_major_e;
            return;
        }
        else if (ch == '.')
        {
            state = &HttpRequestParser::state_version_minor;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_version_minor(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch == '0' || ch == '1')
        {
            ev.onHttpVersion(1, ch - '0');
            state = &HttpRequestParser::state_end0;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http version field");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_end0(char ch)
    {
        if (ch == '\n')
        {
            state = &HttpRequestParser::state_h0;
            return;
        }
        else if (ch == ' ' || ch == '\t' || ch == '\r')
        {
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http request line");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_h0(char ch)
    {
        if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else if (ch > 32 && ch < 127)
        {
            token.reserve(32);
            token = ch;
            state = &HttpRequestParser::state_hfieldname;
            return;
        }
        else if (ch == '\r')
        {
            state = &HttpRequestParser::state_hcr;
            return;
        }
        else if (ch == '\n')
        {
            ev.onEnd();
            state = &HttpRequestParser::state_end;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http header");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_hcr(char ch)
    {
        if (ch == '\n')
        {
            ev.onEnd();
            state = &HttpRequestParser::state_end;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in http header");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_hfieldname(char ch)
    {
        if (ch == ':')
        {
            ev.onKey(token);
            state = &HttpRequestParser::state_hfieldbody0;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            ev.onKey(token);
            state = &HttpRequestParser::state_hfieldnamespace;
            return;
        }
        else if (ch > 32 && ch < 127)
        {
            token += ch;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldname");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_hfieldnamespace(char ch)
    {
        if (ch == ':')
        {
            state = &HttpRequestParser::state_hfieldbody0;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldname");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_hfieldbody0(char ch)
    {
        if (ch == '\r')
        {
            state = &HttpRequestParser::state_hfieldbody_cr;
            return;
        }
        else if (ch == '\n')
        {
            state = &HttpRequestParser::state_hfieldbody_crlf;
            return;
        }
        else if (std::isspace(ch))
        {
            return;
        }
        else if (!std::isspace(ch))
        {
            token.reserve(32);
            token = ch;
            state = &HttpRequestParser::state_hfieldbody;
            return;
        }
    }

    void HttpRequestParser::state_hfieldbody(char ch)
    {
        if (ch == '\r')
        {
            state = &HttpRequestParser::state_hfieldbody_cr;
            return;
        }
        else if (ch == '\n')
        {
            state = &HttpRequestParser::state_hfieldbody_crlf;
            return;
        }
        else
        {
            token += ch;
            return;
        }
    }

    void HttpRequestParser::state_hfieldbody_cr(char ch)
    {
        if (ch == '\n')
        {
          state = &HttpRequestParser::state_hfieldbody_crlf;
          return;
        }
        else
        {
          log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
          state = &HttpRequestParser::state_error;
          return;
        }

    }

    void HttpRequestParser::state_hfieldbody_crlf(char ch)
    {
        if (ch == '\r')
        {
            ev.onValue(token);
            state = &HttpRequestParser::state_hend_cr;
            return;
        }
        else if (ch == '\n')
        {
            ev.onValue(token);
            ev.onEnd();
            state = &HttpRequestParser::state_end;
            return;
        }
        else if (ch == ' ' || ch == '\t')
        {
            token += ch;
            state = &HttpRequestParser::state_hfieldbody;
            return;
        }
        else if (ch > 32 && ch < 127)
        {
            ev.onValue(token);
            token.reserve(32);
            token = ch;
            state = &HttpRequestParser::state_hfieldname;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_hend_cr(char ch)
    {
        if (ch == '\n')
        {
            ev.onEnd();
            state = &HttpRequestParser::state_end;
            return;
        }
        else
        {
            log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
            state = &HttpRequestParser::state_error;
            return;
        }
    }

    void HttpRequestParser::state_end(char ch)
    {
        return;
    }

    void HttpRequestParser::state_error(char ch)
    {
        return;
    }

}

}
