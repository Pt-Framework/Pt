/*
 * Copyright (C) 2010-2013 Marc Boris Duerner
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
#include <stdexcept>
#include <cstddef>

struct pt_regexp;
struct pt_regmatch_t;

namespace Pt {

class RegexSMatch;

/** @brief Invalid regular expression.

    @ingroup Unicode
*/
class PT_API InvalidRegex : public std::runtime_error
{
    public:
        InvalidRegex(const char* msg);

        ~InvalidRegex() throw()
        {}
};

/** @brief Regular Expression for unicode strings.

    The regular expression syntax is a subset of the extended POSIX syntax.
    The following meta characters are supported: . ^ $ < > - [] () | ? + *.
    Characters are escaped using \.

    @ingroup Unicode
*/
class PT_API Regex
{
    public:
        Regex();

        explicit Regex(const Pt::Char* ex);

        explicit Regex(const Pt::String& ex);

        Regex(const Regex& other);

        ~Regex();

        Regex& operator=(const Regex& other);

        /** @brief Matches the regular experession to a string.

            The result @ sm holds pointers into the original string that was
            matched and therefore should not be used after the original string
            was destroyed.
        */
        bool match(const Pt::String& str, RegexSMatch& sm) const;

        bool match(const Pt::String& str) const;

        bool match(const Char* str, RegexSMatch& sm) const;

        bool match(const Char* str) const;

    private:
        pt_regexp* _expr;
};


/** @brief Result of a regular expression match.

    @ingroup Unicode
*/
class PT_API RegexSMatch
{
    friend class Regex;

    public:
        RegexSMatch();

        RegexSMatch(const RegexSMatch& other);

        ~RegexSMatch();

        RegexSMatch& operator=(const RegexSMatch& other);

        bool empty() const;

        /** @brief Returns the number of matches.
        */
        std::size_t size() const;

        std::size_t maxSize() const;

        std::size_t position(std::size_t n = 0) const;

        std::size_t length(std::size_t n = 0) const;

        Pt::String str(std::size_t n = 0) const;

        /** @brief Formats a string according to a format specifier.

            Each occurance of $N in the format specifying string @a str is
            replaced with the N-th element of the match.
        */
        Pt::String format(const Pt::String& str) const;

    private:
        const Char* _str;
        std::size_t _size;
        pt_regmatch_t* _match;
};

} // namespace Pt

#endif // PT_REGEX_H
