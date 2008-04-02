/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
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
#include "Pt/Char.h"
#include "Pt/String.h"
#ifdef PT_STLPORT
#include <sstream>
#endif

namespace std {

//
// ctype facet
//
std::locale::id ctype<Pt::Char>::id;

#if (defined _MSC_VER || defined __QNX__ || defined __xlC__)

ctype<Pt::Char>::ctype(size_t refs)
: ctype_base(refs)
{ }

#else

ctype<Pt::Char>::ctype(size_t refs)
: locale::facet(refs)
{ }

#endif


ctype<Pt::Char>::~ctype()
{
}


bool ctype<Pt::Char>::do_is(mask m, Pt::Char c) const
{
    return m == ctypeMask(c);
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

#if PT_STLPORT
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

num_put<Pt::Char>::num_put(size_t refs) 
: locale::facet(refs), numput_wchar(use_facet<num_put<wchar_t,iter_type_w> >(loc))
{
}

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
	// force writing with hexadecimal value
    ios_base::fmtflags flags = f.flags();
    f.setf(ios_base::hex, ios_base::basefield);
    f.setf(ios_base::showbase);	
    iter_type result = put_val<const void*>(numput_wchar, s, f, fill, val);
    f.flags(flags);
	return result;
}

//
// num_get facet
//
template<class val_type>
static num_get<Pt::Char>::iter_type get_val(const num_get<wchar_t,num_get<Pt::Char>::iter_type_w>& numget_wchar,
                                     num_get<Pt::Char>::iter_type s, num_get<Pt::Char>::iter_type e, ios_base& f,
                                     ios_base::iostate& state, val_type val)
{
    typedef ostreambuf_iterator<wchar_t,char_traits<wchar_t> > oiter_type_w;
    typedef istreambuf_iterator<wchar_t,char_traits<wchar_t> > iiter_type_w;

    // convert Pt::Char stream into wchar_t
    basic_ostringstream<wchar_t> tmpstream;
    oiter_type_w dst(tmpstream);

    while (s != e)
        *dst++ = *s++;

    // take the result as input stream
    basic_string<wchar_t> str = tmpstream.str();

    basic_istringstream<wchar_t> ins(str);
    iiter_type_w begin(ins);
    iiter_type_w end;

    // take over format flags and other settings
    ins.flags(f.flags());
    ins.precision(f.precision());
    ins.width(f.width());
    numget_wchar.get(begin, end, ins, state, val);
    return s;
}

locale::id num_get<Pt::Char>::id;

num_get<Pt::Char>::num_get(size_t refs) 
: locale::facet(refs), numget_wchar(use_facet<num_get<wchar_t,iter_type_w> >(loc))
{
}

#if !defined (_STLP_NO_BOOL)
num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, bool& val) const
{
    return this->do_get(s, e, f, state, val);
}
#endif

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, long& val) const
{
    return this->do_get(s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, unsigned short& val) const
{
    return this->do_get(s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, unsigned int& val) const
{
    return this->do_get(s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, unsigned long& val) const
{
    return this->do_get(s, e, f, state, val);
}

#if defined (_STLP_LONG_LONG)
num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, long long& val) const
{
    return this->do_get(s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, unsigned long long& val) const
{
    return this->do_get(s, e, f, state, val);
}
#endif

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, float& val) const
{
    return this->do_get(s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, double& val) const
{
    return this->do_get(s, e, f, state, val);
}

#if !defined (_STLP_NO_LONG_DOUBLE)
num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, long double& val) const
{
    return this->do_get(s, e, f, state, val);
}
#endif

num_get<Pt::Char>::iter_type num_get<Pt::Char>::get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, void*& val) const
{
    return this->do_get(s, e, f, state, val);
}

#if !defined (_STLP_NO_BOOL)
num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                    ios_base::iostate& state, bool& val) const
{
    return get_val<bool&>(numget_wchar, s, e, f, state, val);
}
#endif

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, long& val) const
{
    return get_val<long&>(numget_wchar, s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, unsigned short& val) const
{
    return get_val<unsigned short&>(numget_wchar, s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, unsigned int& val) const
{
    return get_val<unsigned int&>(numget_wchar, s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, unsigned long& val) const
{
    return get_val<unsigned long&>(numget_wchar, s, e, f, state, val);
}

#if defined (_STLP_LONG_LONG)
num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, long long& val) const
{
    return get_val<long long&>(numget_wchar, s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, unsigned long long& val) const
{
    return get_val<unsigned long long&>(numget_wchar, s, e, f, state, val);
}
#endif

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, float& val) const
{
    return get_val<float&>(numget_wchar, s, e, f, state, val);
}

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, double& val) const
{
    return get_val<double&>(numget_wchar, s, e, f, state, val);
}

#if !defined (_STLP_NO_LONG_DOUBLE)
num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, long double& val) const
{
    return get_val<long double&>(numget_wchar, s, e, f, state, val);
}
#endif

num_get<Pt::Char>::iter_type num_get<Pt::Char>::do_get(iter_type s, iter_type e, ios_base& f,
                                                       ios_base::iostate& state, void*& val) const
{
	// force reading with hexadecimal value
    ios_base::fmtflags flags = f.flags();
    f.setf(ios_base::hex, ios_base::basefield);
    f.setf(ios_base::showbase);	
	iter_type result = get_val<void*&>(numget_wchar, s, e, f, state, val);
	f.flags(flags);
	return result;
}

#endif

//
// numpunct facet
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


Pt::String  numpunct<Pt::Char>::truename() const
{ return this->do_truename(); }


Pt::String  numpunct<Pt::Char>::falsename() const
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
// codecvt facet
//
std::locale::id codecvt<Pt::Char, char, mbstate_t>::id;


#if defined _MSC_VER || __QNX__

codecvt<Pt::Char, char, mbstate_t>::codecvt(size_t ref)
: codecvt_base(ref)
{}

#else

codecvt<Pt::Char, char, mbstate_t>::codecvt(size_t ref)
: locale::facet(ref)
{}

#endif


codecvt<Pt::Char, char, mbstate_t>::~codecvt()
{}

} // namespace std
