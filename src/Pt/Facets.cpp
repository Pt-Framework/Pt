/*
 * Copyright (C) 2004-2013 Marc Boris Duerner
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

#include <Pt/Convert.h>
#include <algorithm>

namespace {

struct HexFormatUL
{
    static const unsigned base = 16;

    HexFormatUL(const char* digtab)
    : _digtab(digtab)
    {}

    Pt::Char minus() const
    { return '-'; }

    Pt::Char toChar(unsigned char n) const
    {
        return _digtab[n]; 
    }
    
    const char* _digtab;
};

template <typename OutIterT, typename CharT>
inline OutIterT putNumber(OutIterT it, const CharT* beg, const CharT* end,
                          std::ios_base::fmtflags flags, 
                          std::streamsize width, CharT fill) 
{
    bool hasSign = *beg == '+' || *beg == '-';

    std::streamsize len = end - beg;
    if (len >= width)
    {
        return std::copy(beg, end, it);
    }

    std::streamsize pad =  width - len;
    std::ios_base::fmtflags dir = flags & std::ios_base::adjustfield;

    if (dir == std::ios_base::left) 
    {
        it = std::copy(beg, end, it);
        for ( ; pad > 0; --pad)  *it++ = fill;
        return it;
    }
    
    if( dir == std::ios_base::internal && hasSign) 
    {
        *it++ = *beg;
        for ( ; pad > 0; --pad)  *it++ = fill;
        return std::copy(beg + 1, end, it);
    }

    // right adjustment
    for (; pad > 0; --pad)  *it++ = fill;
    return std::copy(beg, end, it);
}

template <typename OutIterT, typename T, typename CharT>
inline OutIterT putDecimal(OutIterT it, T i, 
                           std::ios_base::fmtflags flags, 
                           std::streamsize width, CharT fill)
{
    bool showPos = (flags & std::ios_base::showpos) == std::ios_base::showpos;

    // large enough for decimal with a sign
    const std::size_t buflen = (sizeof(T) * 4) + 1;
    CharT buf[buflen];
    
    Pt::DecimalFormat<CharT> fmt;
    CharT* number = Pt::formatInt(buf, buflen, i, fmt);

    CharT first = *number;
    if(showPos && first != '-' && number != buf)
        *(--number) = '+';

    return putNumber(it, number, buf+buflen, flags, width, fill);
}

template <typename OutIterT, typename T, typename CharT>
inline OutIterT putHex(OutIterT it, T i, 
                       std::ios_base::fmtflags flags, 
                       std::streamsize width, CharT fill)
{
    bool showPos = (flags & std::ios_base::showpos) == std::ios_base::showpos;
    bool showBase = (flags & std::ios_base::showbase) == std::ios_base::showbase;
    bool upperCase = (flags & std::ios_base::uppercase) == std::ios_base::uppercase;
    
    const char* digtabL = "0123456789abcdef";
    const char* digtabU = "0123456789ABCDEF";
    
    const char* digtab = digtabL;
    if(upperCase)
        digtab = digtabU;

    HexFormatUL fmt(digtab);

    // large enough for hex number, sign and base
    const std::size_t buflen = (sizeof(T) * 4) + 3;
    CharT buf[buflen];

    CharT* number = Pt::formatInt(buf, buflen, i, fmt);

    CharT first = *number;
    if(showBase && (number - buf >= 2))
    {
        if(first != '-') 
            --number;

        *number-- = 'x';
        *number = '0';

        if(first == '-') 
            *(--number) = first;
    }

    if(showPos && first != '-' && number != buf)
        *(--number) = '+';

    return putNumber(it, number, buf+buflen, flags, width, fill);
}

template <typename OutIterT, typename T, typename CharT>
inline OutIterT putOctal(OutIterT it, T i, 
                         std::ios_base::fmtflags flags, 
                         std::streamsize width, CharT fill)
{
    bool showPos = (flags & std::ios_base::showpos) == std::ios_base::showpos;
    bool showBase = (flags & std::ios_base::showbase) == std::ios_base::showbase;

    // large enough for octal with a sign and base
    const std::size_t buflen = (sizeof(T) * 4) + 2;
    CharT buf[buflen];

    Pt::OctalFormat<CharT> fmt;
    CharT* number = Pt::formatInt(buf, buflen, i, fmt);

    CharT first = *number;
    if(showBase && (number != buf))
    {
        if(first != '-') 
            --number;

        *number = '0';

        if(first == '-') 
            *(--number) = first;
    }

    if(showPos && first != '-' && number != buf)
        *(--number) = '+';

    return putNumber(it, number, buf+buflen, flags, width, fill);
}

template <typename IterT, typename T, typename CharT>
inline IterT putFloat(IterT it, T d, 
                      std::ios_base::fmtflags flags, 
                      std::streamsize width, CharT fill,
                      std::streamsize precision)
{
    bool scientific = (flags & std::ios_base::scientific) == std::ios_base::scientific;
    bool fixed = (flags & std::ios_base::fixed) == std::ios_base::fixed;
    bool leftAdjust = (flags & std::ios_base::left) == std::ios_base::left;
    bool internalAdjust = (flags & std::ios_base::internal) == std::ios_base::internal;
    bool rightAdjust = ! (leftAdjust || internalAdjust);
    
    if( scientific )
    {
        // always one digit before decimal point 
        precision += 1;
    }

    const std::streamsize bufsize = std::numeric_limits<T>::digits10;
    CharT fract[bufsize];

    int i = 0;
    int e = 0;
    Pt::FloatFormat<Pt::Char> fmt;
    std::streamsize fractSize = Pt::formatFloat(fract, bufsize, i, e, d, fmt, int(precision), fixed);

    std::streamsize digits = 0;

    if(scientific)
    {
        // fraction digits, intpart, 3 exp digits, signed e/E
        digits += precision + 5; 
    }
    else if(fixed)
    {
        // digits after decimal point
        digits += precision;
    
        // digits before decimal point
        digits += (e > 0) ? e + 1 : 1;
    }
    else
    {
        if(e < fractSize)
        {
            // digits after decimal point
            digits += fractSize - e;

            // digits before decimal point
            digits += digits < fractSize ? fractSize - digits + 1: 1;
        }
        else
        {
            // digits after decimal point
            digits += (precision > 1) ? 1 : 0;

            // digits before decimal point
            digits += e + 1;
        }
    }

    std::streamsize len = digits;

    bool hasSign = (i < 0) || (flags & std::ios_base::showpos);
    if(hasSign)
        ++len;

    bool hasPoint = (precision > 1) || (flags & std::ios_base::showpoint);
    if(hasPoint)
        len++;
    
    if(rightAdjust) 
        while(len++ < width)
            *it++ = fill;

    if(hasSign)
        *it++ = (i < 0) ? '-' : '+';

    if (internalAdjust) 
        while(len++ < width)
            *it++ = fill;

    i = (i < 0) ? -i : i;
  
    std::streamsize n = 0;

    if(scientific) 
    {
        *it++ = '0' + i;
        --digits;

        if(hasPoint)
            *it++ = '.';

        // 3 exp digits, signed e/E
        digits -= 5;
    }
    else if(e >= 0) // fixed and default
    {
        *it++ = '0' + i;
        --digits;

        for(; n < e; ++n)
            *it++ = (n < fractSize) ? fract[n] : CharT('0');

        digits -= n;

        if(hasPoint)
            *it++ = '.';
    }
    else // e < 0
    {
        *it++ = '0';
        --digits;
        
        if(hasPoint)
            *it++ = '.';

        digits += e;

        while(++e < 0)
            *it++ = '0';

        *it++ = '0' + i;
        --digits;
    }

    for(; digits > 0; ++n, --digits)
        *it++ = (n < fractSize) ?  fract[n] : CharT('0');   

    if(scientific) 
    {
        *it++ = (flags & std::ios_base::uppercase) ? 'E' : 'e';

        CharT sign = '+';
        if(e < 0)
        {
            e = -e;
            sign = '-';
        }
    
        *it++ = sign;

        if(e < 100)
            *it++ = '0';
        if(e < 10)
            *it++ = '0';

        it = putDecimal(it, e, std::ios_base::dec, 0, ' ');
    }

    if (leftAdjust) 
        while ( len++ < width)
            *it++ = fill;

    return it;
}

} // namespace

namespace std {

//
// numpunct facet specialized for Pt::Char
//

locale::id numpunct<Pt::Char>::id;


numpunct<Pt::Char>::numpunct(std::size_t refs)
: locale::facet(refs)
{ }


numpunct<Pt::Char>::~numpunct()
{ }


Pt::Char numpunct<Pt::Char>::decimal_point() const
{ return this->do_decimal_point(); }


Pt::Char numpunct<Pt::Char>::thousands_sep() const
{ return this->do_thousands_sep(); }


string numpunct<Pt::Char>::grouping() const
{ return this->do_grouping(); }


Pt::String numpunct<Pt::Char>::truename() const
{ return this->do_truename(); }


Pt::String numpunct<Pt::Char>::falsename() const
{ return this->do_falsename(); }


Pt::Char numpunct<Pt::Char>::do_decimal_point() const
{ return '.'; }


Pt::Char numpunct<Pt::Char>::do_thousands_sep() const
{ return ','; }


std::string numpunct<Pt::Char>::do_grouping() const
{ return ""; }


Pt::String numpunct<Pt::Char>::do_truename() const
{
    static const Pt::Char truename[] = {'t', 'r', 'u', 'e', '\0'};
    return truename;
}


Pt::String numpunct<Pt::Char>::do_falsename() const
{
    static const Pt::Char falsename[] = {'f', 'a', 'l', 's', 'e', '\0'};
    return falsename;
}

//
// num_put facet specialized for Pt::Char
//

locale::id num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::id;


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, bool val) const
{
    if( 0 == (f.flags() & ios_base::boolalpha) )
        return do_put(s, f, fill, static_cast<long>(val));
    
    typedef Pt::Char char_type;
    const numpunct<char_type>& np = use_facet< numpunct<char_type> >( f.getloc() );
 
    Pt::String str = val ? np.truename() : np.falsename();
 
    streamsize width = f.width(0);
   
    if( str.size() >= static_cast<std::size_t>(width) )
    {
        return std::copy(str.begin(), str.end(), s);
    }

    streamsize pad = width - str.size();
    ios_base::fmtflags dir = f.flags() & ios_base::adjustfield;

    if (dir == ios_base::left) 
    {
       std::copy(str.begin(), str.end(), s);
       std::fill_n(s, pad, fill);
       return s;
    }

    // right/internal padding 
    std::fill_n(s, pad, fill);
    return std::copy(str.begin(), str.end(), s);
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, long val) const
{
	// TODO: grouping
    //const numpunct<char>& np = use_facet<numpunct<char> >(f.getloc());
    //const string& grouping = np.grouping();

    switch (f.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            putDecimal(s, val, f.flags(), f.width(0), fill);
            break;
    }

    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, long long val) const
{
	// TODO: grouping
    //const numpunct<char>& np = use_facet<numpunct<char> >(f.getloc());
    //const string& grouping = np.grouping();

    switch (f.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            putDecimal(s, val, f.flags(), f.width(0), fill);
            break;
    }

    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, unsigned long val) const
{
	// TODO: grouping
    //const numpunct<char>& np = use_facet<numpunct<char> >(f.getloc());
    //const string& grouping = np.grouping();

    switch (f.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            putDecimal(s, val, f.flags(), f.width(0), fill);
            break;
    }

    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, unsigned long long val) const
{
	// TODO: grouping
    //const numpunct<char>& np = use_facet<numpunct<char> >(f.getloc());
    //const string& grouping = np.grouping();
    
    switch (f.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            putDecimal(s, val, f.flags(), f.width(0), fill);
            break;
    }

    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, double val) const
{
    putFloat(s, val, f.flags(), f.width(0), fill, f.precision());
    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, long double val) const
{
    putFloat(s, val, f.flags(), f.width(0), fill, f.precision());
    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, const void* ptr) const
{
    std::size_t val = reinterpret_cast<std::size_t>(ptr);
    putHex(s, val, f.flags(), f.width(0), fill);
    return s;
}

//
// num_get facet specialized for Pt::Char
//

locale::id num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::id;


num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           bool& val) const
{
    if(stream.flags() & ios_base::boolalpha) 
    {
        const numpunct<Pt::Char>& np = use_facet< numpunct<Pt::Char> >(stream.getloc());
        const Pt::String truename  = np.truename();
        const Pt::String falsename = np.falsename();
        bool true_ok  = true;
        bool false_ok = true;

        std::size_t n = 0;
        for ( ; it != end; ++it) 
        {
            Pt::Char c = *it;
            true_ok  = true_ok && (n < truename.size()) && (c == truename[n]);
            false_ok = false_ok && (n < falsename.size()) && (c == falsename[n]);
            ++n;

            if( (! true_ok && ! false_ok) ||
                (true_ok  && n >= truename.size()) ||
                (false_ok && n >= falsename.size()) ) 
            {
                ++it;
                break;
            }
        }

        if (true_ok && n < truename.size())  
            true_ok  = false;

        if (false_ok && n < falsename.size()) 
            false_ok = false;

        if (true_ok || false_ok) 
        {
            state = ios_base::goodbit;
            val = true_ok;
        }
        else
            state = ios_base::failbit;

        if (it == end)
            state |= ios_base::eofbit;
    }
    else 
    {
        long l = 3;
        it = this->do_get(it, end, stream, state, l);
        if( 0 == (state & ios_base::failbit) ) 
        {
            if (l == 0)
                val = false;
            else if (l == 1)
                val = true;
            else
                state |= ios_base::failbit;
        }
    }

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           long& val) const
{
    bool ok = false;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            it = Pt::parseInt(it, end, val, Pt::OctalFormat<Pt::Char>(), ok);
            break;
        case ios_base::hex:
            it = Pt::parseInt(it, end, val, Pt::HexFormat<Pt::Char>(), ok);
            break;
        default:
            it = Pt::parseInt(it, end, val, Pt::DecimalFormat<Pt::Char>(), ok);
            break;
    }

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           long long& val) const
{
    bool ok = false;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            it = Pt::parseInt(it, end, val, Pt::OctalFormat<Pt::Char>(), ok);
            break;
        case ios_base::hex:
            it = Pt::parseInt(it, end, val, Pt::HexFormat<Pt::Char>(), ok);
            break;
        default:
            it = Pt::parseInt(it, end, val, Pt::DecimalFormat<Pt::Char>(), ok);
            break;
    }

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           unsigned short& val) const
{
    bool ok = false;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            it = Pt::parseInt(it, end, val, Pt::OctalFormat<Pt::Char>(), ok);
            break;
        case ios_base::hex:
            it = Pt::parseInt(it, end, val, Pt::HexFormat<Pt::Char>(), ok);
            break;
        default:
            it = Pt::parseInt(it, end, val, Pt::DecimalFormat<Pt::Char>(), ok);
            break;
    }

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           unsigned int& val) const
{
    bool ok = false;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            it = Pt::parseInt(it, end, val, Pt::OctalFormat<Pt::Char>(), ok);
            break;
        case ios_base::hex:
            it = Pt::parseInt(it, end, val, Pt::HexFormat<Pt::Char>(), ok);
            break;
        default:
            it = Pt::parseInt(it, end, val, Pt::DecimalFormat<Pt::Char>(), ok);
            break;
    }

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           unsigned long& val) const
{    
    bool ok = false;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            it = Pt::parseInt(it, end, val, Pt::OctalFormat<Pt::Char>(), ok);
            break;
        case ios_base::hex:
            it = Pt::parseInt(it, end, val, Pt::HexFormat<Pt::Char>(), ok);
            break;
        default:
            it = Pt::parseInt(it, end, val, Pt::DecimalFormat<Pt::Char>(), ok);
            break;
    }

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           unsigned long long& val) const
{
    bool ok = false;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            it = Pt::parseInt(it, end, val, Pt::OctalFormat<Pt::Char>(), ok);
            break;
        case ios_base::hex:
            it = Pt::parseInt(it, end, val, Pt::HexFormat<Pt::Char>(), ok);
            break;
        default:
            it = Pt::parseInt(it, end, val, Pt::DecimalFormat<Pt::Char>(), ok);
            break;
    }

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}


/* NOTE: this could be useful
void Initialize_get_float(const ctype<Pt::Char>& ct, Pt::Char& Plus, Pt::Char& Minus,
                          Pt::Char& pow_e, Pt::Char& pow_E, Pt::Char* digits) 
{
    char ndigits[11] = "0123456789";
    Plus  = ct.widen('+');
    Minus = ct.widen('-');
    pow_e = ct.widen('e');
    pow_E = ct.widen('E');
    ct.widen(ndigits + 0, ndigits + 10, digits);
}
*/
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           float& val) const
{
    // accept scientific and fixed format. The respective maniuplators 
    // are for input only
    bool ok = false;
    it = Pt::parseFloat(it, end, val, ok);

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}


num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           double& val) const
{
    // accept scientific and fixed format. The respective maniuplators 
    // are for input only
    bool ok = false;
    it = Pt::parseFloat(it, end, val, ok);

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           long double& val) const
{
    // accept scientific and fixed format. The respective maniuplators 
    // are for input only
    bool ok = false;
    it = Pt::parseFloat(it, end, val, ok);

    if( ok )
        state = ios_base::goodbit;
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::iter_type
num_get< Pt::Char, istreambuf_iterator<Pt::Char> >::do_get(iter_type it, iter_type end, 
                                                           ios_base& stream, ios_base::iostate& state, 
                                                           void*& val) const
{
    std::size_t addr = 0;
    bool ok = false;
    Pt::parseInt(it, end, addr, Pt::HexFormat<Pt::Char>(), ok);

    if( ok )
    {
        val = reinterpret_cast<void*>(addr);
        state = ios_base::goodbit;
    }
    else
        state = ios_base::failbit;

    if (it == end)
        state |= ios_base::eofbit;

    return it;
}

//
// ctype facet for Pt::Char
//

std::locale::id ctype<Pt::Char>::id;

#if (defined _MSC_VER || defined __QNX__ || defined __xlC__)

ctype<Pt::Char>::ctype(std::size_t refs)
: ctype_base(refs)
{ }

#else

ctype<Pt::Char>::ctype(std::size_t refs)
: locale::facet(refs)
{ }

#endif


ctype<Pt::Char>::~ctype()
{
}


bool ctype<Pt::Char>::do_is(mask m, Pt::Char c) const
{
    return (m & ctypeMask(c)) != 0;
}


const Pt::Char*
ctype<Pt::Char>::do_is(const Pt::Char* begin, const Pt::Char* end, mask* vec) const
{
    for( ; begin < end; ++begin)
    {
        *vec = ctypeMask(*begin);
        ++vec;
    }

    return end;
}


const Pt::Char*
ctype<Pt::Char>::do_scan_is(mask m, const Pt::Char* begin, const Pt::Char* end) const
{
    while( begin != end && !is(m,*begin))
    {
        ++begin;
    }

    return begin;
}


const Pt::Char*
ctype<Pt::Char>::do_scan_not(mask m, const Pt::Char* begin, const Pt::Char* end) const
{
    while( begin != end && is(m,*begin))
    {
        ++begin;
    }

    return begin;
}


Pt::Char
ctype<Pt::Char>::do_toupper(Pt::Char ch) const
{
    return toupper(ch);
}


const Pt::Char*
ctype<Pt::Char>::do_toupper(Pt::Char* begin, const Pt::Char* end) const
{
    for(; begin < end; ++begin)
    {
        *begin = do_toupper(*begin);
    }

    return end;
}


Pt::Char
ctype<Pt::Char>::do_tolower(Pt::Char ch) const
{
    return tolower(ch);
}


const Pt::Char*
ctype<Pt::Char>::do_tolower(Pt::Char* begin, const Pt::Char* end) const
{
    for(; begin < end; ++begin) {
        *begin = do_tolower(*begin);
    }

    return end;
}


Pt::Char ctype<Pt::Char>::do_widen(char ch) const
{
    return Pt::Char(ch);
}


const char* ctype<Pt::Char>::do_widen(const char* begin, const char* end, Pt::Char* dest) const
{
    for(const char* cur = begin; cur < end; ++cur) {
        *dest = do_widen(*cur);
        ++dest;
    }

    return end;
}


char ctype<Pt::Char>::do_narrow(Pt::Char ch, char dfault) const
{
    return ch.narrow(dfault);
}


const Pt::Char*
ctype<Pt::Char>::do_narrow(const Pt::Char* begin, const Pt::Char* end, char dfault, char* dest) const
{
    for(const Pt::Char* cur = begin; cur < end; ++cur) {
        *dest = do_narrow(*cur, dfault);
        ++dest;
    }

    return end;
}

//
// codecvt facet for Char/char
//

std::locale::id codecvt<Pt::Char, char, Pt::MBState>::id;

#if defined _MSC_VER || __QNX__

codecvt<Pt::Char, char, Pt::MBState>::codecvt(std::size_t ref)
: codecvt_base(ref)
{}

#else

codecvt<Pt::Char, char, Pt::MBState>::codecvt(std::size_t ref)
: locale::facet(ref)
{}

#endif

codecvt<Pt::Char, char, Pt::MBState>::~codecvt()
{}

//
// codecvt facet for char/char
//

std::locale::id codecvt<char, char, Pt::MBState>::id;

#if defined _MSC_VER || __QNX__

codecvt<char, char, Pt::MBState>::codecvt(std::size_t ref)
: codecvt_base(ref)
{}

#else

codecvt<char, char, Pt::MBState>::codecvt(std::size_t ref)
: locale::facet(ref)
{}

#endif

codecvt<char, char, Pt::MBState>::~codecvt()
{}

} // namespace std
