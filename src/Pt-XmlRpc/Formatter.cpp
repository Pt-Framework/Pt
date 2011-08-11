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
#include <Pt/SerializationError.h>
#include <limits>
#include <cassert>
#include <cmath>

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
static const Pt::Char XMLRPC_FALSE[]   = { '0', '\0' };
static const Pt::Char XMLRPC_TRUE[]    = { '1', '\0' };

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

        T lsd = i % 10;
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

        T lsd = i % 10;
        i /= 10;
        --psz;
        *psz = digits[lsd];

    } while(i != 0);

    return psz;
}

inline const Pt::Char* double_to_string(Pt::Char* str, size_t n, double d)
{
    const char* digits = "9876543210123456789";
    digits += 9;
    assert( *digits == '0' );

    Pt::Char* end = str + n;
    Pt::Char* psz = end;

    // 1. Test for not-a-number with x != x
    if( d != d ) 
    {
        if(n < 4)
            return 0;

        str[0] = 'n'; str[1] = 'a'; str[2] = 'n'; str[3] = '\0';
        return str;
    }

    //2. Test for infinity by comparing with max
    if( d == std::numeric_limits<double>::infinity() ) 
    {
        if(n < 4)
            return 0;

        str[0] = 'i'; str[1] = 'n'; str[2] = 'f'; str[3] = '\0';
        return str;
    }
    
    //3. Split into integer and fractional parts with modf (<cmath>)
    double intpart = 0;
    double fract = std::modf(d, &intpart);
    int digit = 0;
	size_t places = std::numeric_limits<double>::digits10;
    
    //4. Repeatedly div-mod the integer part to extract the digits
    //   left of the decimal point.
    
    if(intpart == 0)
    {
		if(psz == str)
			return 0;
			
		*(--psz) = '0';
    }
    else
    {
		do
		{
			if(psz == str)
				return 0;
			
			double remain = std::fmod(intpart, 10);
			digit = static_cast<int>( remain );
			intpart -= remain;
			intpart /= 10;
			*(--psz) = digits[digit];
			--places;
		} 
		while(intpart != 0.0);
    }
    
    if(d < 0.0)
    {
		if(psz == str)
			return 0;
		*(--psz) = '-';
    }
    
    size_t count = end - psz;
    std::memmove(str, psz, count * sizeof(Pt::Char));
    psz = str + count;
    
    if(psz == end)
		return 0;

    *psz = '.';
    psz++;

    //5. repeatedly multiply and modf the fractional part to get the 
    //   digits to the right. It will probably be faster to work in base 
    //   10^9 rather than 10.

    do
    {
        fract *= 10;
        fract = modf(fract, &intpart);
        digit = static_cast<int>(intpart);
        char c = digits[digit];

        if(psz == end)
            return 0;
	
        *psz = c;
        ++psz;
    } 
    while(--places != 0 && fract != 0.0);

	//6. add null terminator
    if(psz == end)
         return 0;

    *psz = '\0';
    return str;
}
/*
struct Tester
{
	Tester()
	{
		std::cerr << "-----   TEST   -----" << std::endl;
		{
		Pt::Char buf1[1024];
		const Pt::Char* res = double_to_string(buf1, 1024, 123.1415);
		Pt::String str1(res);
		std::cerr << "R1: " << str1.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}
		
		{
		Pt::Char buf2[1024];
		const Pt::Char* res1 = double_to_string(buf2, 1024, 0.123);
		Pt::String str2(res1);
		std::cerr << "R2: " << str2.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}
		
		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, 0.0);
		Pt::String str3(res3);
		std::cerr << "R3: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}
		
		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, 0.00001);
		Pt::String str3(res3);
		std::cerr << "R4: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}
		
		
		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, 1.0);
		Pt::String str3(res3);
		std::cerr << "R5: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}
		
		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, 10000.0);
		Pt::String str3(res3);
		std::cerr << "R6: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}
		
		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, -123.321);
		Pt::String str3(res3);
		std::cerr << "R6: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}

		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, std::numeric_limits<double>::quiet_NaN());
		Pt::String str3(res3);
		std::cerr << "R6: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}

		{
		Pt::Char buf3[1024];
		const Pt::Char* res3 = double_to_string(buf3, 1024, std::numeric_limits<double>::infinity());
		Pt::String str3(res3);
		std::cerr << "R6: " << str3.narrow() << std::endl;
		std::cerr << "--------------------" << std::endl;
		}	
		
		std::exit(1);
	}
} g_tester;
*/
}

namespace Pt {

namespace XmlRpc {

void Formatter::addString(const char* name, const char* type,
                          const Pt::String& value, const char* id)
{
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING, value.c_str());
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addBool(const char* name, bool value, 
                        const char* id)
{
    _writer->writeStartTag(XMLRPC_VALUE);

    if(value)
        _writer->writeElement(XMLRPC_BOOLEAN, XMLRPC_TRUE);
    else
        _writer->writeElement(XMLRPC_BOOLEAN, XMLRPC_FALSE);

    _writer->writeEndTag(XMLRPC_VALUE);
}

void Formatter::addChar(const char* name, const Pt::Char& value,
                        const char* id)
{
    Pt::Char str[2] = { value, '\0' };

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING , str);
    _writer->writeEndTag(XMLRPC_VALUE);
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

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_INT, num);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addFloat(const char* name, double value, 
                         const char* id)
{
    const size_t bufsize = 64;
    Pt::Char buf[bufsize];
    const Pt::Char* num = double_to_string(buf, bufsize, value);
    if( 0 == num  )
        throw std::logic_error("conversion buffer too small");
    
    //convert(_value, value);
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_DOUBLE, num);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addBytes(const char* name, const char* type,
                         const char* data, size_t length, const char* id)
{
    // TODO: this should be base64 encoded

    _writer->writeStartTag(XMLRPC_VALUE);
    std::string value(data, length);
    _writer->writeElement( Pt::String::widen(type), Pt::String::widen(value) );
    _writer->writeStartTag(XMLRPC_VALUE);
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
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeStartTag(XMLRPC_STRUCT);
}


void Formatter::beginMember(const char* name, const char*, const char*)
{
    _writer->writeStartTag(XMLRPC_MEMBER);
    _writer->writeElement(XMLRPC_NAME, Pt::String::widen(name) );
}


void Formatter::finishMember()
{
    _writer->writeEndTag(XMLRPC_MEMBER);
}


void Formatter::finishObject()
{
    _writer->writeEndTag(XMLRPC_STRUCT);
    _writer->writeEndTag(XMLRPC_VALUE);
}

}

}
