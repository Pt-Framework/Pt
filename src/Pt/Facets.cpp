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



locale::id num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::id;

 
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
num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, const void* ptr) const
{
    std::size_t val = reinterpret_cast<std::size_t>(ptr);
    Pt::putHex(s, val, f.flags(), f.width(0), fill);
    return s;
}




// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, double val) const
// {
//     return s;
// }


// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, bool val) const
// {
//     if(f.flags() & std::ios_base::boolalpha)
//     {
//         const numpunct<char_type>& npct = use_facet< numpunct<char_type> >( f.getloc() );
//         Pt::String out = val ? npct.truename() : npct.falsename();
//         std::copy(out.begin(), out.end(), s);

//         streamsize pad = f.width();
//         if( pad > out.size() )
//         {

//             if ( (f.flags() & ios_base::adjustfield) == ios_base::left)
//             {
//                 std::copy(out.begin(), out.end(), s);
//                 std::fill_n(s, pad, fill);
//             }
//             else
//             {
//                 std::fill_n(s, pad, fill);
//                 std::copy(out.begin(), out.end(), s);
//             }
//         }
//         else
//         {
//             std::copy(out.begin(), out.end(), s);
//         }
//     }
//     else
//     {
//         ++s = val? '1' : '0';
//     }

//     return s;
// }


// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, long val) const
// {
//     return s;
// }


// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::iter_type
// num_put<Pt::Char, ostreambuf_iterator<Pt::Char> >::do_put(iter_type s, ios_base& f, char_type fill, unsigned long val) const
// {
//     return s;
// }




#if PT_STLPORT || defined(_RWSTD_NO_CLASS_PARTIAL_SPEC)
//
// num_put facet
//
template<class val_type>
static num_put<Pt::Char>::iter_type put_val(const num_put<wchar_t,num_put<Pt::Char>::iter_type_w>& numput_wchar,
                                     num_put<Pt::Char>::iter_type s, ios_base& f, num_put<Pt::Char>::char_type fill,
                                     val_type val)
{
    basic_ostringstream<wchar_t> tmp;

    num_put<Pt::Char>::iter_type_w begin(tmp);

    // take over flags and other settings
    tmp.flags(f.flags());
    tmp.precision(f.precision());
    tmp.width(f.width());
    numput_wchar.put(begin, tmp, static_cast<wchar_t>(fill), val);

    basic_string<wchar_t> str = tmp.str();

    basic_string<wchar_t>::iterator srcit = str.begin();
    const size_t len = str.length();
    for (size_t i = 0; i < len; ++i)
        *s++ = *srcit++;

    return s;    
}

locale::id num_put<Pt::Char>::id;

#if defined(_RWSTD_NO_CLASS_PARTIAL_SPEC)
num_put<Pt::Char>::num_put(size_t refs) 
: locale::facet(refs), numput_wchar( use_facet(loc, (num_put<wchar_t>*) 0) ) /// FIXME: baaaad !!!
{
}
#else
num_put<Pt::Char>::num_put(size_t refs)
: locale::facet(refs), numput_wchar(use_facet<num_put<wchar_t,iter_type_w> >(loc))
{
}
#endif

#if !defined (_STLP_NO_BOOL)
num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
              bool val) const
{
    return this->do_put(s, f, fill, val);
}
#endif

num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
              long val) const
{
    return this->do_put(s, f, fill, val);
}

num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
                                                    unsigned long val) const
{
    return this->do_put(s, f, fill, val);
}

#if defined (_STLP_LONG_LONG)
num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
              long long val) const
{
    return this->do_put(s, f, fill, val);
}

num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
              unsigned long long val) const
{
    return this->do_put(s, f, fill, val);
}
#endif

num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
              double val) const
{
    return this->do_put(s, f, fill, val);
}

#if !defined (_STLP_NO_LONG_DOUBLE)
num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
                                                    long double val) const
{
    return this->do_put(s, f, fill, val);
}
#endif

num_put<Pt::Char>::iter_type num_put<Pt::Char>::put(iter_type s, ios_base& f, char_type fill, 
                                                    const void* val) const
{
    return this->do_put(s, f, fill, val);
}

#if !defined (_STLP_NO_BOOL)
num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                    bool val) const
{
    return put_val<bool>(numput_wchar, s, f, fill, val);
}
#endif

num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                    long val) const
{
    return put_val<long>(numput_wchar, s, f, fill, val);
}

num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                       unsigned long val) const
{
    return put_val<unsigned long>(numput_wchar, s, f, fill, val);
}

#if defined (_STLP_LONG_LONG)
num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                    long long val) const
{
    return put_val<long long>(numput_wchar, s, f, fill, val);
}

num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                    unsigned long long val) const
{
    return put_val<unsigned long long>(numput_wchar, s, f, fill, val);
}
#endif

num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                    double val) const
{
    return put_val<double>(numput_wchar, s, f, fill, val);
}

#if !defined (_STLP_NO_LONG_DOUBLE)
num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                       long double val) const
{
    return put_val<long double>(numput_wchar, s, f, fill, val);
}
#endif

num_put<Pt::Char>::iter_type num_put<Pt::Char>::do_put(iter_type s, ios_base& f, char_type fill, 
                                                       const void* val) const
{

// Note:
// The STLport internal routine for serializing a pointer 
// performs hexadecimal conversion i.e. the result is a hexadecimal string.
// The routines for deserializing does NOT expect a hexadecimal string
// Is this a bug in STLport?
// 
// Here we force a numerical conversion because we don't provide a num_get
// facet where we could force hexadecimal deserialization.

// force writing with hexadecimal value
//ios_base::fmtflags flags = f.flags();
//f.setf(ios_base::hex, ios_base::basefield);
//f.setf(ios_base::showbase);	
//iter_type result = put_val<size_t>(numput_wchar, s, f, fill, val_);
//f.flags(flags);
//return result;

#if defined (_STLP_LONG_LONG)
    long long val_ = reinterpret_cast<long long>(val);
#else
    long val_ = reinterpret_cast<long>(val);
#endif
    return do_put(s, f, fill, val_);    
}

locale::id num_get<Pt::Char>::id;

#endif

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
