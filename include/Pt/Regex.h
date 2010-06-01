/*
 * Copyright (C) 2010 Marc Boris Duerner
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

#ifndef PT_REGEX_H
#define PT_REGEX_H

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/SmartPtr.h>
#include <cstdlib>

struct regexp;
struct pt_regmatch_t;

namespace Pt {

class RegexSMatch;

template <typename objectType>
class RegexDestroyPolicy;

template <>
class RegexDestroyPolicy<regexp>
{
    protected:
        void destroy(regexp* expr)
        {
            std::free(expr);
        }
};


class PT_API InvalidRegex : public std::invalid_argument
{
    public:
        InvalidRegex(const std::string& s);

        ~InvalidRegex() throw()
        {}
};


class PT_API Regex
{
      SmartPtr<regexp, ExternalRefCounted<regexp>, RegexDestroyPolicy<regexp> > _expr;

    public:
        explicit Regex(const Pt::Char* ex);

        explicit Regex(const Pt::String& ex);

        bool match(const Pt::String& str, RegexSMatch& smatch) const;

        bool match(const Pt::String& str) const;

        void free()  { _expr = 0; }
};


/// collects matches in a regex
class PT_API RegexSMatch
{
    friend class Regex;

    private:
        Pt::String _str;
        unsigned _size;
        pt_regmatch_t* _match;

    public:
        RegexSMatch();

        ~RegexSMatch();

        /// returns the number of expressions, which were found
        unsigned size() const;

        /// returns the start position of the n-th expression
        unsigned offsetBegin(unsigned n) const;

        /// returns the end position of the n-th expression
        unsigned offsetEnd(unsigned n) const;

        /// returns true if the n-th element is set.
        bool has(unsigned n) const;

        /// returns the n-th element. No range checking is done.
        Pt::String get(unsigned n) const;

        /// replace each occurence of "$n" with the n-th element (n: 0..9).
        Pt::String format(const Pt::String& str) const;

        /// returns the n-th element. No range checking is done.
        Pt::String operator[] (unsigned n) const
        { return get(n); }
};

}

#endif // PT_REGEX_H

