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
#ifndef PT_FACETS_H
#define PT_FACETS_H

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Locale.h>
#include <iostream>
#include <iterator>

namespace std {

// gcc 3.4.x requires a __numpunct_cache
template <typename T>
struct __numpunct_cache;

/** @brief Numpunct localization facet

    @ingroup Unicode
*/
template <>
class PT_API numpunct<Pt::Char> : public locale::facet {
    public:
        typedef Pt::Char char_type;
        typedef basic_string<Pt::Char> string_type;

        // __GLIBCXX__ <= 20051201 && __GLIBCXX__ >= 20040419
        typedef __numpunct_cache<Pt::Char>  __cache_type;

        static locale::id id;
        
        // NOTE: rouguwave solaris
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit numpunct(std::size_t refs = 0);

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

/** @brief Numput localization facet

    @ingroup Unicode
*/
template <>
class PT_API num_put< Pt::Char, 
                      ostreambuf_iterator<Pt::Char> > : public locale::facet 
{
    public:
        typedef Pt::Char char_type;
        typedef ostreambuf_iterator<Pt::Char> iter_type;

        explicit num_put(std::size_t refs = 0)
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

/** @brief Numget localization facet

    @ingroup Unicode
*/
template<>
class PT_API num_get< Pt::Char, 
                      istreambuf_iterator<Pt::Char> > : public locale::facet
{
    public:
        typedef Pt::Char char_type;
        typedef istreambuf_iterator<Pt::Char> iter_type;

        explicit num_get(std::size_t refs = 0)
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

#if (defined _MSC_VER || defined __QNX__ || defined __xlC__)

    /** @brief Ctype localization facet
        @ingroup Unicode
    */
    template <>
    class PT_API ctype< Pt::Char > : public ctype_base {

#else
    /** @brief Ctype localization facet
        @ingroup Unicode
    */
    template <>
    class PT_API ctype<Pt::Char> : public ctype_base, public locale::facet {

#endif

    public:
        typedef ctype_base::mask mask;

        static locale::id id;

        // NOTE: rouguwave solaris
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit ctype(std::size_t refs = 0);

        virtual ~ctype();

        bool is(mask m, Pt::Char c) const
        { return this->do_is(m, c); }

        const Pt::Char* is(const Pt::Char *lo, const Pt::Char *hi, mask *vec) const
        { return this->do_is(lo, hi, vec); }

        const Pt::Char* scan_is(mask m, const Pt::Char* lo, const Pt::Char* hi) const
        { return this->do_scan_is(m, lo, hi); }

        const Pt::Char* scan_not(mask m, const Pt::Char* lo, const Pt::Char* hi) const
        { return this->do_scan_not(m, lo, hi); }

        Pt::Char toupper(Pt::Char c) const
        { return this->do_toupper(c); }

        const Pt::Char* toupper(Pt::Char *lo, const Pt::Char* hi) const
        { return this->do_toupper(lo, hi); }

        Pt::Char tolower(Pt::Char c) const
        { return this->do_tolower(c); }

        const Pt::Char* tolower(Pt::Char* lo, const Pt::Char* hi) const
        { return this->do_tolower(lo, hi); }

        Pt::Char widen(char c) const
        { return this->do_widen(c); }

        const char* widen(const char* lo, const char* hi, Pt::Char* to) const
        { return this->do_widen(lo, hi, to); }

        char narrow(Pt::Char c, char dfault) const
        { return this->do_narrow(c, dfault); }

        const Pt::Char* narrow(const Pt::Char* lo, const Pt::Char* hi,
                            char dfault, char *to) const
        { return this->do_narrow(lo, hi, dfault, to); }

    protected:
        virtual bool do_is(mask m, Pt::Char c) const;

        virtual const Pt::Char* do_is(const Pt::Char* lo, const Pt::Char* hi,
                                    mask* vec) const;

        virtual const Pt::Char* do_scan_is(mask m, const Pt::Char* lo,
                                            const Pt::Char* hi) const;

        virtual const Pt::Char* do_scan_not(mask m, const Pt::Char* lo,
                                            const Pt::Char* hi) const;

        virtual Pt::Char do_toupper(Pt::Char) const;

        virtual const Pt::Char* do_toupper(Pt::Char* lo, const Pt::Char* hi) const;

        virtual Pt::Char do_tolower(Pt::Char) const;

        virtual const Pt::Char* do_tolower(Pt::Char* lo, const Pt::Char* hi) const;

        virtual Pt::Char do_widen(char) const;

        virtual const char* do_widen(const char* lo, const char* hi,
                                    Pt::Char* dest) const;

        virtual char do_narrow(Pt::Char, char dfault) const;

        virtual const Pt::Char* do_narrow(const Pt::Char* lo, const Pt::Char* hi,
                                        char dfault, char* dest) const;
};

#if (defined _MSC_VER || defined __QNX__)

template<>
/** @brief Codecvt localization facet

    @ingroup Unicode
*/
class PT_API codecvt<Pt::Char, char, Pt::MBState> : public codecvt_base {

#else

/** @brief Codecvt localization facet

    @ingroup Unicode
*/
template<>
class PT_API codecvt<Pt::Char, char, Pt::MBState> : public codecvt_base, public locale::facet {

#endif

    public:
        static locale::id id;

        // NOTE: rouguwave solaris
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit codecvt(std::size_t ref = 0);

        virtual ~codecvt();

        codecvt_base::result out(Pt::MBState& state, 
                                 const Pt::Char* from,
                                 const Pt::Char* from_end, 
                                 const Pt::Char*& from_next,
                                 char* to, 
                                 char* to_end, 
                                 char*& to_next) const
        { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

        codecvt_base::result unshift(Pt::MBState& state, 
                                     char* to, 
                                     char* to_end,
                                     char*& to_next) const
        { return this->do_unshift(state, to, to_end, to_next); }

        codecvt_base::result in(Pt::MBState& state, 
                                const char* from,
                                const char* from_end, 
                                const char*& from_next,
                                Pt::Char* to, 
                                Pt::Char* to_end, 
                                Pt::Char*& to_next) const
        { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

        int encoding() const
        { return this->do_encoding(); }

        bool always_noconv() const
        { return this->do_always_noconv(); }

        int length(Pt::MBState& state, const char* from,
                   const char* end, std::size_t max) const
        { return this->do_length(state, from, end, max); }

        int max_length() const
        { return this->do_max_length(); }

    protected:
        virtual codecvt_base::result do_out(Pt::MBState& state, 
                                            const Pt::Char* from,
                                            const Pt::Char* from_end, 
                                            const Pt::Char*& from_next,
                                            char* to, 
                                            char* to_end, 
                                            char*& to_next) const = 0;

        virtual codecvt_base::result do_unshift(Pt::MBState& state, 
                                                char* to,
                                                char* to_end, 
                                                char*& to_next) const = 0;

        virtual codecvt_base::result do_in(Pt::MBState& state,
                                           const char* from, 
                                           const char* from_end,
                                           const char*& from_next, 
                                           Pt::Char* to, 
                                           Pt::Char* to_end,
                                           Pt::Char*& to_next) const = 0;

        virtual int do_encoding() const throw() = 0;

        virtual bool do_always_noconv() const throw() = 0;

        virtual int do_length(Pt::MBState&, 
                              const char* from,
                              const char* end, 
                              std::size_t max) const = 0;

        virtual int do_max_length() const throw() = 0;
};

#if (defined _MSC_VER || defined __QNX__)

template<>
class PT_API codecvt<char, char, Pt::MBState> : public codecvt_base {

#else

template<>
class PT_API codecvt<char, char, Pt::MBState> : public codecvt_base, public locale::facet {

#endif

    public:
        static locale::id id;

        // NOTE: rouguwave solaris
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit codecvt(std::size_t ref = 0);

        virtual ~codecvt();

        codecvt_base::result out(Pt::MBState& state, 
                                 const char* from,
                                 const char* from_end, 
                                 const char*& from_next,
                                 char* to, 
                                 char* to_end, 
                                 char*& to_next) const
        { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

        codecvt_base::result unshift(Pt::MBState& state, 
                                     char* to, 
                                     char* to_end,
                                     char*& to_next) const
        { return this->do_unshift(state, to, to_end, to_next); }

        codecvt_base::result in(Pt::MBState& state, 
                                const char* from,
                                const char* from_end, 
                                const char*& from_next,
                                char* to, char* to_end, 
                                char*& to_next) const
        { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

        int encoding() const
        { return this->do_encoding(); }

        bool always_noconv() const
        { return this->do_always_noconv(); }

        int length(Pt::MBState& state, const char* from,
                   const char* end, std::size_t max) const
        { return this->do_length(state, from, end, max); }

        int max_length() const
        { return this->do_max_length(); }

    protected:
        virtual codecvt_base::result do_out(Pt::MBState& state, 
                                            const char* from,
                                            const char* from_end, 
                                            const char*& from_next,
                                            char* to, 
                                            char* to_end, 
                                            char*& to_next) const = 0;

        virtual codecvt_base::result do_unshift(Pt::MBState& state, 
                                                char* to,
                                                char* to_end, 
                                                char*& to_next) const = 0;

        virtual codecvt_base::result do_in(Pt::MBState& state,
                                           const char* from, 
                                           const char* from_end,
                                           const char*& from_next, 
                                           char* to, 
                                           char* to_end,
                                           char*& to_next) const = 0;

        virtual int do_encoding() const throw() = 0;

        virtual bool do_always_noconv() const throw() = 0;

        virtual int do_length(Pt::MBState&, 
                              const char* from,
                              const char* end, 
                              std::size_t max) const = 0;

        virtual int do_max_length() const throw() = 0;
}; 

} // namespace std


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

} // namespace Pt

#endif
