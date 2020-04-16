/* Copyright (C) 2020-2020 Aloysius Indrayanto
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

#ifndef PT_FORMAT_STRING_FS_H
#define PT_FORMAT_STRING_FS_H

namespace Pt {


//
// Macros for generating function arguments
//
#define FS_ARG_NAME_N(I) const FormatStringValue& a##I
#define FS_ARG_NAME_X(X) FS_ARG_NAME_##X

#define FS_ARG_NAME_1  FS_ARG_NAME_N(1)
#define FS_ARG_NAME_2  FS_ARG_NAME_1,  FS_ARG_NAME_N(2)
#define FS_ARG_NAME_3  FS_ARG_NAME_2,  FS_ARG_NAME_N(3)
#define FS_ARG_NAME_4  FS_ARG_NAME_3,  FS_ARG_NAME_N(4)
#define FS_ARG_NAME_5  FS_ARG_NAME_4,  FS_ARG_NAME_N(5)
#define FS_ARG_NAME_6  FS_ARG_NAME_5,  FS_ARG_NAME_N(6)
#define FS_ARG_NAME_7  FS_ARG_NAME_6,  FS_ARG_NAME_N(7)
#define FS_ARG_NAME_8  FS_ARG_NAME_7,  FS_ARG_NAME_N(8)
#define FS_ARG_NAME_9  FS_ARG_NAME_8,  FS_ARG_NAME_N(9)
#define FS_ARG_NAME_10 FS_ARG_NAME_9,  FS_ARG_NAME_N(10)
#define FS_ARG_NAME_11 FS_ARG_NAME_10, FS_ARG_NAME_N(11)
#define FS_ARG_NAME_12 FS_ARG_NAME_11, FS_ARG_NAME_N(12)
#define FS_ARG_NAME_13 FS_ARG_NAME_12, FS_ARG_NAME_N(13)
#define FS_ARG_NAME_14 FS_ARG_NAME_13, FS_ARG_NAME_N(14)
#define FS_ARG_NAME_15 FS_ARG_NAME_14, FS_ARG_NAME_N(15)
#define FS_ARG_NAME_16 FS_ARG_NAME_15, FS_ARG_NAME_N(16)
#define FS_ARG_NAME_17 FS_ARG_NAME_16, FS_ARG_NAME_N(17)
#define FS_ARG_NAME_18 FS_ARG_NAME_17, FS_ARG_NAME_N(18)
#define FS_ARG_NAME_19 FS_ARG_NAME_18, FS_ARG_NAME_N(19)
#define FS_ARG_NAME_20 FS_ARG_NAME_19, FS_ARG_NAME_N(20)
#define FS_ARG_NAME_21 FS_ARG_NAME_20, FS_ARG_NAME_N(21)
#define FS_ARG_NAME_22 FS_ARG_NAME_21, FS_ARG_NAME_N(22)
#define FS_ARG_NAME_23 FS_ARG_NAME_22, FS_ARG_NAME_N(23)
#define FS_ARG_NAME_24 FS_ARG_NAME_23, FS_ARG_NAME_N(24)
#define FS_ARG_NAME_25 FS_ARG_NAME_24, FS_ARG_NAME_N(25)
#define FS_ARG_NAME_26 FS_ARG_NAME_25, FS_ARG_NAME_N(26)
#define FS_ARG_NAME_27 FS_ARG_NAME_26, FS_ARG_NAME_N(27)
#define FS_ARG_NAME_28 FS_ARG_NAME_27, FS_ARG_NAME_N(28)
#define FS_ARG_NAME_29 FS_ARG_NAME_28, FS_ARG_NAME_N(29)
#define FS_ARG_NAME_30 FS_ARG_NAME_29, FS_ARG_NAME_N(30)
#define FS_ARG_NAME_31 FS_ARG_NAME_30, FS_ARG_NAME_N(31)
#define FS_ARG_NAME_32 FS_ARG_NAME_31, FS_ARG_NAME_N(32)


//
// Macros for storing function arguments to std::vector<>
//
#define FS_ARG_STOR_N(I) args[I - 1] = &a##I
#define FS_ARG_STOR_X(X) FS_ARG_STOR_##X

#define FS_ARG_STOR_1  FS_ARG_STOR_N(1)
#define FS_ARG_STOR_2  FS_ARG_STOR_1  ; FS_ARG_STOR_N(2)
#define FS_ARG_STOR_3  FS_ARG_STOR_2  ; FS_ARG_STOR_N(3)
#define FS_ARG_STOR_4  FS_ARG_STOR_3  ; FS_ARG_STOR_N(4)
#define FS_ARG_STOR_5  FS_ARG_STOR_4  ; FS_ARG_STOR_N(5)
#define FS_ARG_STOR_6  FS_ARG_STOR_5  ; FS_ARG_STOR_N(6)
#define FS_ARG_STOR_7  FS_ARG_STOR_6  ; FS_ARG_STOR_N(7)
#define FS_ARG_STOR_8  FS_ARG_STOR_7  ; FS_ARG_STOR_N(8)
#define FS_ARG_STOR_9  FS_ARG_STOR_8  ; FS_ARG_STOR_N(9)
#define FS_ARG_STOR_10 FS_ARG_STOR_9  ; FS_ARG_STOR_N(10)
#define FS_ARG_STOR_11 FS_ARG_STOR_10 ; FS_ARG_STOR_N(11)
#define FS_ARG_STOR_12 FS_ARG_STOR_11 ; FS_ARG_STOR_N(12)
#define FS_ARG_STOR_13 FS_ARG_STOR_12 ; FS_ARG_STOR_N(13)
#define FS_ARG_STOR_14 FS_ARG_STOR_13 ; FS_ARG_STOR_N(14)
#define FS_ARG_STOR_15 FS_ARG_STOR_14 ; FS_ARG_STOR_N(15)
#define FS_ARG_STOR_16 FS_ARG_STOR_15 ; FS_ARG_STOR_N(16)
#define FS_ARG_STOR_17 FS_ARG_STOR_16 ; FS_ARG_STOR_N(17)
#define FS_ARG_STOR_18 FS_ARG_STOR_17 ; FS_ARG_STOR_N(18)
#define FS_ARG_STOR_19 FS_ARG_STOR_18 ; FS_ARG_STOR_N(19)
#define FS_ARG_STOR_20 FS_ARG_STOR_19 ; FS_ARG_STOR_N(20)
#define FS_ARG_STOR_21 FS_ARG_STOR_20 ; FS_ARG_STOR_N(21)
#define FS_ARG_STOR_22 FS_ARG_STOR_21 ; FS_ARG_STOR_N(22)
#define FS_ARG_STOR_23 FS_ARG_STOR_22 ; FS_ARG_STOR_N(23)
#define FS_ARG_STOR_24 FS_ARG_STOR_23 ; FS_ARG_STOR_N(24)
#define FS_ARG_STOR_25 FS_ARG_STOR_24 ; FS_ARG_STOR_N(25)
#define FS_ARG_STOR_26 FS_ARG_STOR_25 ; FS_ARG_STOR_N(26)
#define FS_ARG_STOR_27 FS_ARG_STOR_26 ; FS_ARG_STOR_N(27)
#define FS_ARG_STOR_28 FS_ARG_STOR_27 ; FS_ARG_STOR_N(28)
#define FS_ARG_STOR_29 FS_ARG_STOR_28 ; FS_ARG_STOR_N(29)
#define FS_ARG_STOR_30 FS_ARG_STOR_29 ; FS_ARG_STOR_N(30)
#define FS_ARG_STOR_31 FS_ARG_STOR_30 ; FS_ARG_STOR_N(31)
#define FS_ARG_STOR_32 FS_ARG_STOR_31 ; FS_ARG_STOR_N(32)


//
// Macros for generating front-end functions
//
#define FS_GENERATE_FORMAT_FUNCTION(X)                                                 \
    inline Pt::String format(const Pt::String& fmt, FS_ARG_NAME_X(X))                  \
    {                                                                                  \
        if(fmt.empty()) return "";                                                     \
                                                                                       \
        std::vector<const FormatStringValue*> args(X);                                 \
        FS_ARG_STOR_X(X);                                                              \
                                                                                       \
        Pt::String resultBuffer;                                                       \
        FormatString(fmt, &args)(resultBuffer);                                        \
        return resultBuffer;                                                           \
    }                                                                                  \
                                                                                       \
    inline Pt::String format(const FormatStringValue::locale_t& loc,                   \
                             const Pt::String& fmt, FS_ARG_NAME_X(X))                  \
    {                                                                                  \
        if(fmt.empty()) return "";                                                     \
                                                                                       \
        std::vector<const FormatStringValue*> args(X);                                 \
        FS_ARG_STOR_X(X);                                                              \
                                                                                       \
        Pt::String resultBuffer;                                                       \
        FormatString(fmt, &args)(resultBuffer, &loc);                                  \
        return resultBuffer;                                                           \
    }                                                                                  \
                                                                                       \
    template <typename OutputIt>                                                       \
    inline OutputIt& format_to(OutputIt& out, const Pt::String& fmt, FS_ARG_NAME_X(X)) \
    {                                                                                  \
        if(fmt.empty()) return out;                                                    \
                                                                                       \
        std::vector<const FormatStringValue*> args(X);                                 \
        FS_ARG_STOR_X(X);                                                              \
                                                                                       \
        Pt::String resultBuffer;                                                       \
        FormatString(fmt, &args)(resultBuffer);                                        \
        out << resultBuffer;                                                           \
                                                                                       \
        return out;                                                                    \
    }                                                                                  \
                                                                                       \
    template <typename OutputIt>                                                       \
    inline OutputIt& format_to(OutputIt& out, const FormatStringValue::locale_t& loc,  \
                               const Pt::String& fmt, FS_ARG_NAME_X(X))                \
    {                                                                                  \
        if(fmt.empty()) return out;                                                    \
                                                                                       \
        std::vector<const FormatStringValue*> args(X);                                 \
        FS_ARG_STOR_X(X);                                                              \
                                                                                       \
        Pt::String resultBuffer;                                                       \
        FormatString(fmt, &args)(resultBuffer, &loc);                                  \
        out << resultBuffer;                                                           \
                                                                                       \
        return out;                                                                    \
    }

    
//
// Front-end functions that accept only the format string
//
inline Pt::String format(const Pt::String& fmt)
{
    if(fmt.empty()) return "";

    Pt::String resultBuffer;
    FormatString(fmt, 0)(resultBuffer);
    return resultBuffer;
}


inline Pt::String format(const FormatStringValue::locale_t& loc, const Pt::String& fmt)
{
    if(fmt.empty()) return "";

    Pt::String resultBuffer;
    FormatString(fmt, 0)(resultBuffer, &loc);
    return resultBuffer;
}


template <typename OutputIt>
inline OutputIt& format_to(OutputIt& out, const Pt::String& fmt)
{
    if(fmt.empty()) return out;

    Pt::String resultBuffer;
    FormatString(fmt, 0)(resultBuffer);
    out << resultBuffer;

    return out;
}


template <typename OutputIt>
inline OutputIt& format_to(OutputIt& out, const FormatStringValue::locale_t& loc, const Pt::String& fmt)
{
    if(fmt.empty()) return out;

    Pt::String resultBuffer;
    FormatString(fmt, 0)(resultBuffer, &loc);
    out << resultBuffer;

    return out;
}


//
// Generate front-end functions that accept the format string and some arguments
//
FS_GENERATE_FORMAT_FUNCTION(1)
FS_GENERATE_FORMAT_FUNCTION(2)
FS_GENERATE_FORMAT_FUNCTION(3)
FS_GENERATE_FORMAT_FUNCTION(4)
FS_GENERATE_FORMAT_FUNCTION(5)
FS_GENERATE_FORMAT_FUNCTION(6)
FS_GENERATE_FORMAT_FUNCTION(7)
FS_GENERATE_FORMAT_FUNCTION(8)
FS_GENERATE_FORMAT_FUNCTION(9)
FS_GENERATE_FORMAT_FUNCTION(10)
FS_GENERATE_FORMAT_FUNCTION(11)
FS_GENERATE_FORMAT_FUNCTION(12)
FS_GENERATE_FORMAT_FUNCTION(13)
FS_GENERATE_FORMAT_FUNCTION(14)
FS_GENERATE_FORMAT_FUNCTION(15)
FS_GENERATE_FORMAT_FUNCTION(16)
FS_GENERATE_FORMAT_FUNCTION(17)
FS_GENERATE_FORMAT_FUNCTION(18)
FS_GENERATE_FORMAT_FUNCTION(19)
FS_GENERATE_FORMAT_FUNCTION(20)
FS_GENERATE_FORMAT_FUNCTION(21)
FS_GENERATE_FORMAT_FUNCTION(22)
FS_GENERATE_FORMAT_FUNCTION(23)
FS_GENERATE_FORMAT_FUNCTION(24)
FS_GENERATE_FORMAT_FUNCTION(25)
FS_GENERATE_FORMAT_FUNCTION(26)
FS_GENERATE_FORMAT_FUNCTION(27)
FS_GENERATE_FORMAT_FUNCTION(28)
FS_GENERATE_FORMAT_FUNCTION(29)
FS_GENERATE_FORMAT_FUNCTION(30)
FS_GENERATE_FORMAT_FUNCTION(31)
FS_GENERATE_FORMAT_FUNCTION(32)


} // namespace


#endif


