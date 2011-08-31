/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
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
#ifndef PT_FACETS_H
#define PT_FACETS_H

#include <Pt/Api.h>
#include <Pt/String.h>
#include <iostream>
#include <iterator>

namespace std {

// gcc 3.4.x violates the c++ standard by requiring a __numpunct_cache
template <typename T>
class __numpunct_cache;

/** @brief Numpunct localization facet
    @ingroup Unicode
*/
template <>
class PT_API numpunct<Pt::Char> : public locale::facet {
    public:
        typedef Pt::Char char_type;
        typedef basic_string<Pt::Char> string_type;

        //#if __GLIBCXX__ <= 20051201 && __GLIBCXX__ >= 20040419
        typedef __numpunct_cache<Pt::Char>  __cache_type;
        //#endif

        static locale::id id;
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit numpunct(size_t refs = 0);

        virtual ~numpunct();

        char_type decimal_point() const;

        char_type thousands_sep() const;

        string grouping() const;

        string_type truename() const;

        string_type falsename() const;

    protected:
        virtual char_type do_decimal_point() const;

        virtual char_type do_thousands_sep() const;

        virtual string do_grouping() const;

        virtual string_type do_truename() const;

        virtual string_type do_falsename() const;
};


template <>
class PT_API num_put< Pt::Char, 
                      ostreambuf_iterator<Pt::Char> > : public locale::facet 
{
    public:
        typedef Pt::Char char_type;
        typedef ostreambuf_iterator<Pt::Char> iter_type;

        explicit num_put(size_t refs = 0)
        : locale::facet(refs)
        { }

        iter_type put(iter_type s, ios_base& f, char_type fill, bool val) const
        { return this->do_put( s, f, fill, val ); }

        // NOTE: rouguwave solaris
        iter_type put(iter_type s, ios_base& f, char_type fill, int val) const
        { return this->do_put( s, f, fill, long(val) ); }

        // NOTE: rouguwave solaris
        iter_type put(iter_type s, ios_base& f, char_type fill, unsigned val) const
        { return this->do_put( s, f, fill, (unsigned long)(val) ); }

        iter_type put(iter_type s, ios_base& f, char_type fill, long val) const
        { return this->do_put( s, f, fill, val ); }

        iter_type put(iter_type s, ios_base& f, char_type fill, unsigned long val) const
        { return this->do_put( s, f, fill, val ); }

        iter_type put(iter_type s, ios_base& f, char_type fill, long long val) const
        { return this->do_put( s, f, fill, val ); }

        iter_type put(iter_type s, ios_base& f, char_type fill, unsigned long long val) const
        { return this->do_put( s, f, fill, val ); }

        iter_type put(iter_type s, ios_base& f, char_type fill, double val) const
        { return this->do_put( s, f, fill, val ); }

        iter_type put(iter_type s, ios_base& f, char_type fill, long double val) const
        { return this->do_put( s, f, fill, val ); }

         iter_type put(iter_type s, ios_base& f, char_type fill, const void* val) const
        { return this->do_put( s, f, fill, val ); }

        static locale::id id;

        // NOTE: rouguwave solaris
        virtual locale::id& __get_id (void) const 
        { return id; }

    protected:
        virtual ~num_put()
        {}

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, bool val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, long val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, unsigned long val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, long long val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, unsigned long long val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, double val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill, long double  val) const;

        virtual iter_type do_put(iter_type s, ios_base& f, char_type fill,  const void*) const;
};


template<>
class PT_API num_get< Pt::Char, 
                      istreambuf_iterator<Pt::Char> > : public locale::facet
{
    public:
        typedef Pt::Char char_type;
        typedef istreambuf_iterator<Pt::Char> iter_type;

        explicit num_get(size_t refs = 0)
        : locale::facet(refs)
        {}

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, bool& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, long& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, long long int& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, unsigned short& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, unsigned int& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, unsigned long& val) const
        { return this->do_get(it, end, f, s, val); }
        
        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, unsigned long long & val) const
        { return this->do_get(it, end, f, s, val); }
        
        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, float& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, double& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, long double& val) const
        { return this->do_get(it, end, f, s, val); }

        iter_type get(iter_type it, iter_type end, ios_base& f, ios_base::iostate& s, void*& val) const
        { return this->do_get(it, end, f, s, val); }

        static locale::id id;

        // NOTE: rouguwave solaris
        virtual locale::id& __get_id (void) const
        { return id; }

    protected:
        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, bool&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, long&) const;
        
        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, long long&) const;
        
        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, unsigned short&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, unsigned int&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, unsigned long&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, unsigned long long&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, float&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, double&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, long double&) const;

        virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, void*&) const;
};

}


namespace Pt {

static std::ios_base::Init pt_stream_init;

static struct PT_API InitLocale
{
    InitLocale()
    {
        
        std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
        
    }
} pt_init_locale;

}

#endif
