/*
 * Copyright (C) 2004-2011 Marc Boris Duerner
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
#ifndef PT_LOCALE_H
#define PT_LOCALE_H

#include <Pt/Api.h>
#include <Pt/Types.h>

#ifdef _WIN32_WCE
    // WinCE does not provide locale-classes
#else
    #define PT_WITH_STD_LOCALE 1
#endif

#ifdef PT_WITH_STD_LOCALE
#include <locale>
#else

namespace std {

class codecvt_base
{
    public:
        enum { ok, partial, error, noconv };
        typedef int result;
        
        virtual ~codecvt_base()
        { }
};

template <typename I, typename E, typename S>
class codecvt : public std::codecvt_base
{
    public:
        typedef I intern_type;
        typedef E extern_type;
        typedef S state_type; 
    
    public: 
        codecvt(std::size_t ref = 0)
        {}
        
        virtual ~codecvt()
        { }
        
        codecvt_base::result out(state_type& state, 
                                 const intern_type* from,
                                 const intern_type* from_end, 
                                 const intern_type*& from_next,
                                 extern_type* to, 
                                 extern_type* to_end, 
                                 extern_type*& to_next) const
        { return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

        codecvt_base::result unshift(state_type& state, 
                                     extern_type* to, 
                                     extern_type* to_end,
                                     extern_type*& to_next) const
        { return this->do_unshift(state, to, to_end, to_next); }

        codecvt_base::result in(state_type& state, 
                                const extern_type* from,
                                const extern_type* from_end, 
                                const extern_type*& from_next,
                                intern_type* to, 
                                intern_type* to_end, 
                                intern_type*& to_next) const
        { return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

        int encoding() const
        { return this->do_encoding(); }

        bool always_noconv() const
        { return this->do_always_noconv(); }

        int length(state_type& state, const extern_type* from,
                   const extern_type* end, std::size_t max) const
        { return this->do_length(state, from, end, max); }

        int max_length() const
        { return this->do_max_length(); }
    
    protected:
        virtual result do_in(state_type& s, const extern_type* fromBegin,
                             const extern_type* fromEnd, const extern_type*& fromNext,
                             intern_type* toBegin, intern_type* toEnd, intern_type*& toNext) const = 0;

        virtual result do_out(state_type& s, const intern_type* fromBegin,
                              const intern_type* fromEnd, const intern_type*& fromNext,
                              extern_type* toBegin, extern_type* toEnd, extern_type*& toNext) const = 0;

        virtual bool do_always_noconv() const = 0;

        virtual int do_length(state_type& s, const extern_type* fromBegin, 
                              const extern_type* fromEnd, std::size_t max) const = 0;

        virtual int do_max_length() const = 0;

        virtual std::codecvt_base::result do_unshift(state_type&, 
                                                     extern_type*, 
                                                     extern_type*, 
                                                     extern_type*&) const = 0;

        virtual int do_encoding() const = 0;
};

class ctype_base
{
    public:
        enum {
            alpha  = 1 << 5,
            cntrl  = 1 << 2,
            digit  = 1 << 6,
            lower  = 1 << 4,
            print  = 1 << 1,
            punct  = 1 << 7,
            space  = 1 << 0,
            upper  = 1 << 3,
            xdigit = 1 << 8,
            alnum  = alpha | digit,
            graph  = alnum | punct
        };

        typedef unsigned short mask;

        ctype_base(std::size_t _refs = 0)
        { }
};

}

#endif

#endif
