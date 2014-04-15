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
        //! @brief Construct with message.
        InvalidRegex(const char* msg);

        //! @brief Destructor.
        ~InvalidRegex() throw()
        {}
};

/** @brief Regular Expressions for Unicode Strings.

    The Pt::Regex class allows to match a string pattern in unicode text. It
    resembles the std::basic_regex class and can be used to support systems,
    where std::basic_regex is not available in the standard C++ implementation.
    The syntax for the match pattern is similar to the extended POSIX syntax.
    The following table shows the special characters that can be used to write
    regular expressions:
    
    <table style="width:600px; margin:20px; padding:6px; background-color: #eff0f0;">
        <tr>
            <td><b>.</b></td>
            <td>Any character</td>
        </tr>
        <tr>
            <td><b>[ ]</b></td>
            <td>A character in a given set</td>
        </tr>
        <tr>
            <td><b>[^ ]</b></td>
            <td>A character not in a given set</td>
        </tr>
        <tr>
            <td><b>^</b></td>
            <td>Begin of line</td>
        </tr>
        <tr>
            <td><b>$</b></td>
            <td>End of line</td>
        </tr>
        <tr>
            <td><b>\\&lt;</b></td>
            <td>Begin of a word</td>
        </tr>
        <tr>
            <td><b>\\&gt;</b></td>
            <td>End of a word</td>
        </tr>
        <tr>
            <td><b>( )</b></td>
            <td>A marked subexpression</td>
        </tr>
        <tr>
            <td><b>*</b></td>
            <td>Matches the preceding element zero or more times</td>
        </tr>
        <tr>
            <td><b>?</b></td>
            <td>Matches the preceding element zero or one time</td>
        </tr>
        <tr>
            <td><b>+</b></td>
            <td>Matches the preceding element one or more times</td>
        </tr>
        <tr>
            <td><b>|</b></td>
            <td>Matches either the expression before or after the operator</td>
        </tr>
        <tr>
            <td><b>\ </b></td>
            <td>Escapes the next character</td>
        </tr>
    </table>

    The regular expression is constructed from a unicode string, either a
    Pt::String or a null-terminated sequence of unicode characters of type
    Pt::Char. It can then be used to match it against unicode strings as
    shown in the next example:

    @code
    Pt::String expr = L"[hc]ats";
    Pt::Regex regex(expr);

    Pt::String str1 = L"I like cats!";
    Pt::String str2 = L"I like hats!";
    Pt::String str3 = L"I like bats!";

    // this does match
    bool matched = regex.match(str1);

    // this does also match
    matched = regex.match(str2);

    // this does not match
    matched = regex.match(str3);
    @endcode

    It is also possibe to match a regular expression against a unicode input
    string and find out what tokens in the string actually matched. The
    @link Pt::Regex::match() match()@endlink member function has an overload,
    which fills a Pt::RegexSMatch with the result. Note that the first result
    at index 0 is always the input string itself. The following example
    illustrates this:

    @code
    Pt::String expr = L"([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)";
    Pt::Regex regex(expr);

    Pt::String str = L"My IP address is 192.168.0.77";

    Pt::RegexSMatch smatch;
    bool matched = regex.match(str, smatch);
    if(matched)
    {
        std::cout << "IP: " << smatch.str(1).narrow() << std::endl;
    }
    else
    {
        std::cout << "No IP in " << smatch.str(0).narrow() << std::endl;
    }
    @endcode

    @ingroup Unicode
*/
class PT_API Regex
{
    public:
        //! @brief Default Constructor.
        Regex();

        //! @brief Construct from regex string.
        explicit Regex(const Pt::Char* ex);

        //! @brief Construct from regex string.
        explicit Regex(const Pt::String& ex);

        //! @brief Copy constructor.
        Regex(const Regex& other);

        //! @brief Destructor.
        ~Regex();

        //! @brief Assignment operator.
        Regex& operator=(const Regex& other);

        /** @brief Matches the regular experession to a string.

            The result @a sm holds pointers into the original string that was
            matched and therefore should not be used after the original string
            was destroyed.
        */
        bool match(const Pt::String& str, RegexSMatch& sm) const;

        //! @brief Returns true if string matches.
        bool match(const Pt::String& str) const;

        /** @brief Matches the regular experession to a string.

            The result @ sm holds pointers into the original string that was
            matched and therefore should not be used after the original string
            was destroyed.
        */
        bool match(const Char* str, RegexSMatch& sm) const;

        //! @brief Returns true if string matches.
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
        //! @brief Default Constructor.
        RegexSMatch();

        //! @brief Copy constructor.
        RegexSMatch(const RegexSMatch& other);

        //! @brief Destructor.
        ~RegexSMatch();

        //! @brief Assignment operator.
        RegexSMatch& operator=(const RegexSMatch& other);

        //! @brief Returns true if no match.
        bool empty() const;

        /** @brief Returns the number of matches.
        */
        std::size_t size() const;

        /** @brief Returns the max number of matches the implementation allows.
        */
        std::size_t maxSize() const;

        /** @brief Returns the position of the nth match.
        */
        std::size_t position(std::size_t n = 0) const;

        /** @brief Returns the size of the nth match.
        */
        std::size_t length(std::size_t n = 0) const;

        /** @brief Returns the nth match.
        */
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
