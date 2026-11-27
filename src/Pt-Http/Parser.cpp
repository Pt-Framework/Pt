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

#include "Parser.h"
#include <Pt/Http/HttpError.h>
#include <cctype>
#include <cstring>

#define PT_LOG_DEBUG(e)
#define PT_LOG_WARN(e)

namespace Pt {

namespace Http {

namespace {

std::string chartoprint(char ch)
{
    const static char hex[] = "0123456789abcdef";
    const unsigned char uch = static_cast<unsigned char>(ch);
    if (std::isprint(uch))
        return std::string(1, '\'') + ch + '\'';
    else
        return std::string("'\\x") + hex[(uch >> 4) & 0xf] + hex[uch & 0xf] + '\'';
}

inline bool isFieldValueChar(unsigned char ch)
{
    return ch == ' ' || ch == '\t'
        || (ch >= 0x21 && ch <= 0x7E)
        || ch >= 0x80;
}


inline bool isHexDigit(char ch)
{
    return (ch >= '0' && ch <= '9')
        || (ch >= 'A' && ch <= 'F')
        || (ch >= 'a' && ch <= 'f');
}

inline unsigned valueOfHexDigit(char ch)
{
    return ch >= '0' && ch <= '9' ? ch - '0'
            : ch >= 'a' && ch <= 'f' ? ch - 'a' + 10
            : ch >= 'A' && ch <= 'F' ? ch - 'A' + 10
            : 0;
}

} // namespace


void HeaderParser::Event::onMethod(const std::string& method)
{
}

void HeaderParser::Event::onUrl(const std::string& url)
{
}


void HeaderParser::Event::onUrlParam(const std::string& q)
{
}


void HeaderParser::Event::onHttpVersion(unsigned major, unsigned minor)
{
}


void HeaderParser::Event::onKey(const std::string& key)
{
}


void HeaderParser::Event::onValue(const std::string& value)
{
}


void HeaderParser::Event::onHttpReturn(unsigned ret, const std::string& text)
{
}


void HeaderParser::Event::onEnd()
{
}


void HeaderParser::MessageHeaderEvent::onHttpVersion(unsigned major, unsigned minor)
{
        _header->setVersion(major, minor);
}


void HeaderParser::MessageHeaderEvent::onKey(const std::string& key)
{
    if(key.size() >= MaxKeySize)
        throw HttpError("invalid HTTP message");

    std::memcpy(_key, key.data(), key.size());
    _key[key.size()] = '\0';
}


void HeaderParser::MessageHeaderEvent::onValue(const std::string& value)
{
    if( MessageHeader::isMatch(_key, "Content-Length") )
    {
        if( _header->has("Transfer-Encoding") || ! MessageHeader::isDigits(value.c_str()) )
            throw HttpError("invalid HTTP message");

        const char* current = _header->get("Content-Length");
        if(current && std::strcmp(current, value.c_str()) != 0)
            throw HttpError("invalid HTTP message");
    }
    else if( MessageHeader::isMatch(_key, "Transfer-Encoding") )
    {
        if( _header->has("Content-Length") )
            throw HttpError("invalid HTTP message");
    }
    else if( MessageHeader::isMatch(_key, "Host") )
    {
        const bool http11 = _header->versionMajor() > 1 ||
                           (_header->versionMajor() == 1 && _header->versionMinor() >= 1);

        if( http11 && (_header->has("Host") || value.empty()) )
            throw HttpError("invalid HTTP message");
    }

    _header->add(_key, value.c_str());
}


void HeaderParser::advance(std::streambuf& sb)
{
    std::streamsize avail = sb.in_avail();

    if(avail < 0)
    {
        state = &HeaderParser::state_error;
    }

    while(avail-- > 0)
    {
        if( parse(sb.sbumpc()) )
            break;
    }
}


void HeaderParser::state_cmd0(char ch)
{
    if (MessageHeader::isTokenChar(ch))
    {
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_cmd;
        return;
    }
    else if (ch != ' ' && ch != '\t')
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in method");
        state = &HeaderParser::state_error;
        return;
    }
    else
    {
        state = &HeaderParser::state_cmd;
        return;
    }
}


void HeaderParser::state_cmd(char ch)
{
    if (MessageHeader::isTokenChar(ch))
    {
        token += ch;
        return;
    }
    else if (ch == ' ')
    {
        PT_LOG_DEBUG("method=" << token);
        ev.onMethod(token);
        state = &HeaderParser::state_url0;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in method");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_url0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch > ' ')
    {
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_url;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in url");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_url(char ch)
{
    if (ch == '?')
    {
        PT_LOG_DEBUG("url=" << token);
        ev.onUrl(token);
        token.clear();
        token.reserve(32);
        state = &HeaderParser::state_qparam;
        return;
    }
    else if (ch == ' ' || ch == '\t')
    {
        PT_LOG_DEBUG("url=" << token);
        ev.onUrl(token);
        token.clear();
        token.reserve(32);
        state = &HeaderParser::state_protocol0;
        return;
    }
    else if (ch == '%')
    {
        token += ch;
        state = &HeaderParser::state_urlesc;
        return;
    }
    else if (ch > ' ')
    {
        token += ch;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in url");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_urlesc(char ch)
{
    if (isHexDigit(ch))
    {
        if (token.size() >= 2 && token[token.size() - 2] == '%')
        {
            unsigned v = (valueOfHexDigit(token[token.size() - 1]) << 4) | valueOfHexDigit(ch);
            if(v == 0)
            {
                state = &HeaderParser::state_error;
                return;
            }

            token[token.size() - 2] = static_cast<char>(v);
            token.resize(token.size() - 1);
            state = &HeaderParser::state_url;
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
        PT_LOG_WARN("invalid hex digit " << chartoprint(ch) << " in url");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_qparam(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        PT_LOG_DEBUG("queryString=" << token);
        ev.onUrlParam(token);
        token.clear();
        token.reserve(32);
        state = &HeaderParser::state_protocol0;
        return;
    }
    else if (ch == '+')
    {
        token += ' ';
        return;
    }
    else
    {
        token += ch;
        return;
    }
}


void HeaderParser::state_protocol0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (std::isalpha(static_cast<unsigned char>(ch)))
    {
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_protocol;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http protocol field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_protocol(char ch)
{
    if (ch == ' ' || ch == '\t' || ch == '/')
    {
        if (token != "HTTP")
        {
            PT_LOG_WARN("invalid protocol " << token << " in http protocol field");
            state = &HeaderParser::state_error;
            return;
        }
        else
        {
            state = (ch == '/' ? &HeaderParser::state_version_major : &HeaderParser::state_version0);
            return;
        }
    }
    else if (std::isalpha(static_cast<unsigned char>(ch)))
    {
        token += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http protocol field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_version0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch == '/')
    {
        state = &HeaderParser::state_version_major;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_version_major(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch == '1')
    {
        state = &HeaderParser::state_version_major_e;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_version_major_e(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        state = &HeaderParser::state_version_major_e;
        return;
    }
    else if (ch == '.')
    {
        state = &HeaderParser::state_version_minor;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_version_minor(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch == '0' || ch == '1')
    {
        ev.onHttpVersion(1, ch - '0');
        state = &HeaderParser::state_end0;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_end0(char ch)
{
    if (ch == '\n')
    {
        state = &HeaderParser::state_h0;
        return;
    }
    else if (ch == ' ' || ch == '\t' || ch == '\r')
    {
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http request line");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_h0(char ch)
{
    if (MessageHeader::isTokenChar(ch))
    {
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_hfieldname;
        return;
    }
    else if (ch == '\r')
    {
        state = &HeaderParser::state_hcr;
        return;
    }
    else if (ch == '\n')
    {
        ev.onEnd();
        state = &HeaderParser::state_end;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http header");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_hcr(char ch)
{
    if (ch == '\n')
    {
        ev.onEnd();
        state = &HeaderParser::state_end;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http header");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_hfieldname(char ch)
{
    if (ch == ':')
    {
        ev.onKey(token);
        token.clear();
        state = &HeaderParser::state_hfieldbody0;
        return;
    }
    else if (MessageHeader::isTokenChar(ch))
    {
        token += ch;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in fieldname");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_hfieldbody0(char ch)
{
    if (ch == '\r')
    {
        state = &HeaderParser::state_hfieldbody_cr;
        return;
    }
    else if (ch == '\n')
    {
        state = &HeaderParser::state_hfieldbody_crlf;
        return;
    }
    else if (MessageHeader::isOws(ch))
    {
        return;
    }
    else if (isFieldValueChar(static_cast<unsigned char>(ch)))
    {
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_hfieldbody;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in fieldbody");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_hfieldbody(char ch)
{
    if (ch == '\r')
    {
        state = &HeaderParser::state_hfieldbody_cr;
        return;
    }
    else if (ch == '\n')
    {
        state = &HeaderParser::state_hfieldbody_crlf;
        return;
    }
    else if (isFieldValueChar(static_cast<unsigned char>(ch)))
    {
        token += ch;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in fieldbody");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_hfieldbody_cr(char ch)
{
    if (ch == '\n')
    {
        state = &HeaderParser::state_hfieldbody_crlf;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in fieldbody");
        state = &HeaderParser::state_error;
        return;
    }

}


void HeaderParser::state_hfieldbody_crlf(char ch)
{
    if (ch == '\r')
    {
        pushValue();
        state = &HeaderParser::state_hend_cr;
        return;
    }
    else if (ch == '\n')
    {
        pushValue();
        ev.onEnd();
        state = &HeaderParser::state_end;
        return;
    }
    else if (MessageHeader::isTokenChar(ch))
    {
        pushValue();
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_hfieldname;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in fieldbody");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_hend_cr(char ch)
{
    if (ch == '\n')
    {
        ev.onEnd();
        state = &HeaderParser::state_end;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in fieldbody");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_protocol0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (std::isalpha(static_cast<unsigned char>(ch)))
    {
        token.reserve(32);
        token = ch;
        state = &HeaderParser::state_cl_protocol;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http protocol field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_protocol(char ch)
{
    if (ch == ' ' || ch == '\t' || ch == '/')
    {
        if (token != "HTTP")
        {
            PT_LOG_WARN("invalid protocol " << token << " in http protocol field");
            state = &HeaderParser::state_error;
            return;
        }
        else
        {
            state = (ch == '/' ? &HeaderParser::state_cl_version_major : &HeaderParser::state_cl_version0);
            return;
        }
    }
    else if (std::isalpha(static_cast<unsigned char>(ch)))
    {
        token += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http protocol field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_version0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch == '/')
    {
        state = &HeaderParser::state_cl_version_major;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_version_major(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch == '1')
    {
        state = &HeaderParser::state_cl_version_major_e;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_version_major_e(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        state = &HeaderParser::state_cl_version_major_e;
        return;
    }
    else if (ch == '.')
    {
        state = &HeaderParser::state_cl_version_minor;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http version field");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_version_minor(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (ch == '0' || ch == '1')
    {
        ev.onHttpVersion(1, ch - '0');
        state = &HeaderParser::state_cl_httpresult0;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http result");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_httpresult0(char ch)
{
    if (ch == ' ' || ch == '\t')
    {
        return;
    }
    else if (std::isdigit(static_cast<unsigned char>(ch)))
    {
        value = (ch - '0');
        state = &HeaderParser::state_cl_httpresult;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in http result");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_cl_httpresult(char ch)
{
    if (std::isdigit(static_cast<unsigned char>(ch)))
    {
        value = value * 10 + (ch - '0');
        return;
    }
    else if (ch == ' ' || ch == '\t')
    {
        token.clear();
        token.reserve(32);
        state = &HeaderParser::state_cl_httpresulttext;
    }
}


void HeaderParser::state_cl_httpresulttext(char ch)
{
    if (ch == '\r')
    {
        ev.onHttpReturn(value, token);
        state = &HeaderParser::state_cl_httpresult_cr;
        return;
    }
    else if (ch == '\n')
    {
        ev.onHttpReturn(value, token);
        state = &HeaderParser::state_h0;
        return;
    }
    else if (token.empty() && (ch == ' ' || ch == '\t'))
    {
        return;
    }
    else
    {
        token += ch;
        return;
    }
}


void HeaderParser::state_cl_httpresult_cr(char ch)
{
    if (ch == '\n')
    {
        state = &HeaderParser::state_h0;
        return;
    }
    else
    {
        PT_LOG_WARN("invalid character " << chartoprint(ch) << " in requestheader");
        state = &HeaderParser::state_error;
        return;
    }
}


void HeaderParser::state_end(char ch)
{
    return;
}


void HeaderParser::state_error(char ch)
{
    return;
}


void HeaderParser::pushValue()
{
    // trailing whitespace is removed here instead of in the state machine
    std::size_t n = token.size();
    while(n > 0 && MessageHeader::isOws(token[n - 1]))
        --n;

    token.resize(n);

    ev.onValue(token);
}


bool HeaderParser::isStartLine() const
{
    return state == &HeaderParser::state_cmd0
        || state == &HeaderParser::state_cmd
        || state == &HeaderParser::state_url0
        || state == &HeaderParser::state_url
        || state == &HeaderParser::state_urlesc
        || state == &HeaderParser::state_qparam
        || state == &HeaderParser::state_protocol0
        || state == &HeaderParser::state_protocol
        || state == &HeaderParser::state_version0
        || state == &HeaderParser::state_version_major
        || state == &HeaderParser::state_version_major_e
        || state == &HeaderParser::state_version_minor
        || state == &HeaderParser::state_end0
        || state == &HeaderParser::state_cl_protocol0
        || state == &HeaderParser::state_cl_protocol
        || state == &HeaderParser::state_cl_version0
        || state == &HeaderParser::state_cl_version_major
        || state == &HeaderParser::state_cl_version_major_e
        || state == &HeaderParser::state_cl_version_minor
        || state == &HeaderParser::state_cl_httpresult0
        || state == &HeaderParser::state_cl_httpresult
        || state == &HeaderParser::state_cl_httpresulttext
        || state == &HeaderParser::state_cl_httpresult_cr;
}


bool HeaderParser::isHeader() const
{
    return state == &HeaderParser::state_h0
        || state == &HeaderParser::state_hcr
        || state == &HeaderParser::state_hfieldname
        || state == &HeaderParser::state_hfieldbody0
        || state == &HeaderParser::state_hfieldbody
        || state == &HeaderParser::state_hfieldbody_cr
        || state == &HeaderParser::state_hfieldbody_crlf
        || state == &HeaderParser::state_hend_cr;
}

} // namespace

} // namespace
