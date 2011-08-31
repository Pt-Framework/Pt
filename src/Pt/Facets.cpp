/*
 * Copyright (C) 2004-2010 Marc Boris Duerner
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

namespace std {

//
// numpunct facet specialized for Pt::Char
//

locale::id numpunct<Pt::Char>::id;


numpunct<Pt::Char>::numpunct(size_t refs)
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
   
    if( str.size() >= static_cast<size_t>(width) )
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
            Pt::putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            Pt::putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            Pt::putDecimal(s, val, f.flags(), f.width(0), fill);
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
            Pt::putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            Pt::putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            Pt::putDecimal(s, val, f.flags(), f.width(0), fill);
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
            Pt::putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            Pt::putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            Pt::putDecimal(s, val, f.flags(), f.width(0), fill);
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
            Pt::putOctal(s, val, f.flags(), f.width(0), fill);
            break;
        case ios_base::hex:
            Pt::putHex(s, val, f.flags(), f.width(0), fill);
            break;
        default:
            Pt::putDecimal(s, val, f.flags(), f.width(0), fill);
            break;
    }

    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, double val) const
{
    Pt::putFloat(s, val, f.flags(), f.width(0), fill, f.precision());
    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, long double val) const
{
    Pt::putFloat(s, val, f.flags(), f.width(0), fill, f.precision());
    return s;
}


num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, const void* ptr) const
{
    std::size_t val = reinterpret_cast<std::size_t>(ptr);
    Pt::putHex(s, val, f.flags(), f.width(0), fill);
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

        size_t n = 0;
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
    std::size_t base = 10;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            base = 8;
            break;
        case ios_base::hex:
            base = 16;
            break;
        default:
            base = 10;
            break;
    }

    bool ok = false;
    it = Pt::getSigned(it, end, ok, val, base);

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
    std::size_t base = 10;
    switch(stream.flags() & ios_base::basefield) 
    {
        case ios_base::oct:
            base = 8;
            break;
        case ios_base::hex:
            base = 16;
            break;
        default:
            base = 10;
            break;
    }

    bool ok = false;
    it = Pt::getSigned(it, end, ok, val, base);

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
    it = Pt::getUnsigned(it, end, ok, val);

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
    it = Pt::getUnsigned(it, end, ok, val);

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
    it = Pt::getUnsigned(it, end, ok, val);

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
    it = Pt::getUnsigned(it, end, ok, val);

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
                                                           float& val) const
{
    bool ok = false;
    it = Pt::getFloat(it, end, ok, val);

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
    bool ok = false;
    it = Pt::getFloat(it, end, ok, val);

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
    bool ok = false;
    it = Pt::getFloat(it, end, ok, val);

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

    Pt::getUnsigned(it, end, ok, addr, 16);

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

} // namespace std

#if PT_STLPORT

_STLP_BEGIN_NAMESPACE
_STLP_MOVE_TO_PRIV_NAMESPACE

void  _Initialize_get_float( const ctype<Pt::Char>& ct,
        Pt::Char& Plus, Pt::Char& Minus,
        Pt::Char& pow_e, Pt::Char& pow_E,
        Pt::Char* digits) {
  char ndigits[11] = "0123456789";
  Plus  = ct.widen('+');
  Minus = ct.widen('-');
  pow_e = ct.widen('e');
  pow_E = ct.widen('E');
  ct.widen(ndigits + 0, ndigits + 10, digits);
}    

_STLP_MOVE_TO_STD_NAMESPACE
_STLP_END_NAMESPACE  

#endif
