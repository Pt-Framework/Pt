/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Convert.h>
#include <cassert>

namespace  {

static const Pt::Char XMLRPC_VALUE[]   = { 'v', 'a', 'l', 'u', 'e', '\0' };
static const Pt::Char XMLRPC_INT[]     = { 'i', 'n', 't', '\0' };
static const Pt::Char XMLRPC_DOUBLE[]  = { 'd', 'o', 'u', 'b', 'l', 'e', '\0' };
static const Pt::Char XMLRPC_STRING[]  = { 's', 't', 'r', 'i', 'n', 'g', '\0' };
static const Pt::Char XMLRPC_BOOLEAN[] = { 'b', 'o', 'o', 'l', 'e', 'a', 'n', '\0' };
static const Pt::Char XMLRPC_STRUCT[]  = { 's', 't', 'r', 'u', 'c', 't', '\0' };
static const Pt::Char XMLRPC_MEMBER[]  = { 'm', 'e', 'm', 'b', 'e', 'r', '\0' };
static const Pt::Char XMLRPC_NAME[]    = { 'n', 'a', 'm', 'e', '\0' };
static const Pt::Char XMLRPC_ARRAY[]   = { 'a', 'r', 'r', 'a', 'y', '\0' };
static const Pt::Char XMLRPC_DATA[]    = { 'd', 'a', 't', 'a', '\0' };
static const Pt::Char XMLRPC_FALSE[]   = { 'f', 'a', 'l', 's', 'e', '\0' };
static const Pt::Char XMLRPC_TRUE[]    = { 't', 'r', 'u', 'e', '\0' };

template <typename T>
inline const Pt::Char* signed_integer_to_string(Pt::Char* buf, size_t n, T i)
{
    const bool negative = i < 0;

    const char* digits = "9876543210123456789";
    digits += 9;
    assert( *digits == '0' );

    if(0 == n)
        return 0;

    Pt::Char* psz = buf + n - 1;
    *psz = 0;

    do
    {
        if(psz == buf)
            return 0;

        signed lsd = i % 10;
        i /= 10;
        --psz;
        *psz = digits[lsd];

    } while(i != 0);

    if(negative)
    {
        if(psz == buf)
            return 0;

        --psz;
        *psz = '-';
    }

    return psz;
}

template <typename T>
inline const Pt::Char* unsigned_integer_to_string(Pt::Char* buf, size_t n, T i)
{
    const char* digits = "0123456789";
    assert( *digits == '0' );

    if(0 == n)
        return 0;

    Pt::Char* psz = buf + n - 1;
    *psz = 0;

    do
    {
        if(psz == buf)
            return 0;

        signed lsd = i % 10;
        i /= 10;
        --psz;
        *psz = digits[lsd];

    } while(i != 0);

    return psz;
}

}

namespace Pt {

namespace XmlRpc {

void Formatter::addString(const char* name, const char* type,
                          const Pt::String& value, const char* id)
{
    _writer->writeStartElement(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING, value.c_str());
    _writer->writeEndElement();
}


void Formatter::addBool(const char* name, bool value, 
                        const char* id)
{
    _writer->writeStartElement(XMLRPC_VALUE );

    if(value)
        _writer->writeElement(XMLRPC_BOOLEAN, XMLRPC_TRUE);
    else
        _writer->writeElement(XMLRPC_BOOLEAN, XMLRPC_FALSE);

    _writer->writeEndElement();
}

void Formatter::addChar(const char* name, const Pt::Char& value,
                        const char* id)
{
    Pt::Char str[2] = { value, '\0' };

    _writer->writeStartElement(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING , str);
    _writer->writeEndElement();
}

void Formatter::addInt(const char* name, long long value, 
                       const char* id)
{
    const size_t bufsize = (sizeof(value) * 4) + 4;
    Pt::Char buf[bufsize];
    const Pt::Char* num = signed_integer_to_string(buf, bufsize, value);
    if( 0 == num  )
        throw std::logic_error("conversion buffer too small");

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_INT, num);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addUInt(const char* name, unsigned long long value, 
                        const char* id)
{
    const size_t bufsize = (sizeof(value) * 4) + 4;
    Pt::Char buf[bufsize];
    const Pt::Char* num = unsigned_integer_to_string(buf, bufsize, value);
    if( 0 == num  )
        throw std::logic_error("conversion buffer too small");

    _writer->writeStartElement(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_INT, num);
    _writer->writeEndElement();
}


void Formatter::addFloat(const char* name, double value, 
                         const char* id)
{
    convert(_value, value);
    _writer->writeStartElement(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_DOUBLE, _value.c_str());
    _writer->writeEndElement();
}


void Formatter::addBytes(const char* name, const char* type,
                         const char* data, size_t length, const char* id)
{
    // TODO: this should be base64 encoded

    _writer->writeStartElement(XMLRPC_VALUE);
    std::string value(data, length);
    _writer->writeElement( Pt::String::widen(type), Pt::String::widen(value) );
    _writer->writeEndElement();
}


void Formatter::addReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void Formatter::beginArray(const char*, const char*,
                           const char*)
{
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeStartTag(XMLRPC_ARRAY);
    _writer->writeStartTag(XMLRPC_DATA);
}


void Formatter::beginElement(const char* type, const char* id)
{
}


void Formatter::finishElement()
{
}


void Formatter::finishArray()
{
    _writer->writeEndTag(XMLRPC_DATA);
    _writer->writeEndTag(XMLRPC_ARRAY);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::beginObject(const char* name, const char* type,
                            const char* id)
{
    _writer->writeStartElement(XMLRPC_VALUE);
    _writer->writeStartElement(XMLRPC_STRUCT);
}


void Formatter::beginMember(const char* name, const char* type, 
                            const char* id)
{
    _writer->writeStartElement(XMLRPC_MEMBER);
    _writer->writeElement(XMLRPC_NAME, Pt::String::widen(name) );
}


void Formatter::finishMember()
{
    _writer->writeEndElement();
}


void Formatter::finishObject()
{
    _writer->writeEndElement();
    _writer->writeEndElement();
}


void Formatter::finish()
{
    _writer->writeEndElement();
}

}

}
