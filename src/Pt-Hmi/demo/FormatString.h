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

#ifndef PT_FORMAT_STRING_H
#define PT_FORMAT_STRING_H


#include <vector>

#include <Pt/String.h>


namespace Pt {


//
// Format string error
//
class PT_API FormatStringError : public std::runtime_error
{
    public:
        explicit FormatStringError(const std::string& msg);

        explicit FormatStringError(const char* msg);

        virtual ~FormatStringError() throw()
        {}
};


//
// Format string specifier
//
struct FormatStringSpec {
    // fill-and-align(optional) sign(optional) #(optional) 0(optional) width(optional) precision(optional) L(optional) type(optional)
    char fill;       // fill character
    char align;      // < > ^
    char sign;       // + - [space]
    bool altForm;    // #
    bool zeroPad;    // 0
    int  width;      // minimum field width (default 0)
    int  precision;  // floating-point precision (default 6)
    bool locale;     // use locale-specific formatting
    char type;       // none/s b B c d o x X a A e E f/F g G p

    inline FormatStringSpec()
    : fill     (0    ) // default none
    , align    (0    ) // default '<' for non number and '>' for number
    , sign     ('-'  )
    , altForm  (false)
    , zeroPad  (false)
    , width    (0    )
    , precision(6    )
    , locale   (false)
    , type     (0    ) // default copy as string
    {}
};


//
// Format string argument and it's formatter
//
class PT_API FormatStringArg {

    public:
        inline FormatStringArg(Pt::int8_t         p) :_type(AT_I8 ) { _valuePOD.i8  = p;             }
        inline FormatStringArg(Pt::uint8_t        p) :_type(AT_U8 ) { _valuePOD.u8  = p;             }
        inline FormatStringArg(Pt::int16_t        p) :_type(AT_I16) { _valuePOD.i16 = p;             }
        inline FormatStringArg(Pt::uint16_t       p) :_type(AT_U16) { _valuePOD.u16 = p;             }
        inline FormatStringArg(Pt::int32_t        p) :_type(AT_I32) { _valuePOD.i32 = p;             }
        inline FormatStringArg(Pt::uint32_t       p) :_type(AT_U32) { _valuePOD.u32 = p;             }
        inline FormatStringArg(Pt::int64_t        p) :_type(AT_I64) { _valuePOD.i64 = p;             }
        inline FormatStringArg(Pt::uint64_t       p) :_type(AT_U64) { _valuePOD.u64 = p;             }
        inline FormatStringArg(float              p) :_type(AT_F  ) { _valuePOD.f   = p;             }
        inline FormatStringArg(double             p) :_type(AT_D  ) { _valuePOD.d   = p;             }
        inline FormatStringArg(long double        p) :_type(AT_LD ) { _valuePOD.ld  = p;             }
        inline FormatStringArg(bool               p) :_type(AT_B  ) { _valuePOD.b   = p;             }
        inline FormatStringArg(const void*        p) :_type(AT_P  ) { _valuePOD.p   = p;             }
        inline FormatStringArg(const char*        p) :_type(AT_S  ) { _valueString  = Pt::String(p); }
        inline FormatStringArg(const std::string& p) :_type(AT_S  ) { _valueString  = p.c_str();     }
        inline FormatStringArg(const Pt::String&  p) :_type(AT_S  ) { _valueString  = p;             }

        const Pt::String operator()(const FormatStringSpec& fs, const std::numpunct<Pt::Char>& numpunct) const;

    private:
        enum ArgType {
            AT_I8,  AT_U8,  // 8  bits integers
            AT_I16, AT_U16, // 16 bits integers
            AT_I32, AT_U32, // 32 bits integers
            AT_I64, AT_U64, // 64 bits integers
            AT_F,           // float
            AT_D,           // double
            AT_LD,          // long double
            AT_B,           // boolean
            AT_P,           // pointer
            AT_S            // string
        };

        union ArgValue {
            Pt::int8_t   i8;
            Pt::uint8_t  u8;
            Pt::int16_t  i16;
            Pt::uint16_t u16;
            Pt::int32_t  i32;
            Pt::uint32_t u32;
            Pt::int64_t  i64;
            Pt::uint64_t u64;
            float        f;
            double       d;
            long double  ld;
            bool         b;
            const void*  p;
        };

        ArgType    _type;
        ArgValue   _valuePOD;
        Pt::String _valueString;
};


//
// The main string formatter
//
class PT_API FormatString {
    public:
        inline FormatString(const char* format, const std::vector<const FormatStringArg*>& args)
        : _format(format), _args(args)
        {}

        inline FormatString(const std::string& format, const std::vector<const FormatStringArg*>& args)
        : _format(format.c_str()), _args(args)
        {}

        inline FormatString(const Pt::Char* format, const std::vector<const FormatStringArg*>& args)
        : _format(format), _args(args)
        {}

        inline FormatString(const Pt::String& format, const std::vector<const FormatStringArg*>& args)
        : _format(format), _args(args)
        {}

        const Pt::String operator()() const;

    private:
        const Pt::String&                          _format;
        const std::vector<const FormatStringArg*>& _args;
};



//
// Front-ends functions
//
#define FS_ARG_NAME_N(I) const FormatStringArg& a##I
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

#define FS_GENERATE_FORMAT_FUNCTION(X)                                          \
    inline Pt::String format_string(const Pt::String& format, FS_ARG_NAME_X(X)) \
    {                                                                           \
        std::vector<const FormatStringArg*> args(X);                            \
        FS_ARG_STOR_X(X);                                                       \
        FormatString fstr(format, args);                                        \
        return fstr();                                                          \
    }

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


} // namespace


#endif

