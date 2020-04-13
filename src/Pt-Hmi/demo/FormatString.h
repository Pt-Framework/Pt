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
// Format-string error
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
// Format-string specifier
//
struct PT_API FormatStringSpec {
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
    { reset(); }

    inline void reset()
    {
        fill      = 0;     // default none
        align     = 0;     // default '<' for non number and '>' for number
        sign      = '-';
        altForm   = false;
        zeroPad   = false;
        width     = 0;
        precision = 6;
        locale    = false;
        type      = 0;     // default copy as string
    }
};


//
// Format-string argument and its corresponding formatter
//
class PT_API FormatStringArg {
    public:
        // Constructors (one for each data type)
        inline FormatStringArg(Pt::int8_t         p) : _fmtFun(&FormatStringArg::ff_I8 ) { _valPOD.i8  = p;             }
        inline FormatStringArg(Pt::uint8_t        p) : _fmtFun(&FormatStringArg::ff_U8 ) { _valPOD.u8  = p;             }
        inline FormatStringArg(Pt::int16_t        p) : _fmtFun(&FormatStringArg::ff_I16) { _valPOD.i16 = p;             }
        inline FormatStringArg(Pt::uint16_t       p) : _fmtFun(&FormatStringArg::ff_U16) { _valPOD.u16 = p;             }
        inline FormatStringArg(Pt::int32_t        p) : _fmtFun(&FormatStringArg::ff_I32) { _valPOD.i32 = p;             }
        inline FormatStringArg(Pt::uint32_t       p) : _fmtFun(&FormatStringArg::ff_U32) { _valPOD.u32 = p;             }
        inline FormatStringArg(Pt::int64_t        p) : _fmtFun(&FormatStringArg::ff_I64) { _valPOD.i64 = p;             }
        inline FormatStringArg(Pt::uint64_t       p) : _fmtFun(&FormatStringArg::ff_U64) { _valPOD.u64 = p;             }
        inline FormatStringArg(float              p) : _fmtFun(&FormatStringArg::ff_F  ) { _valPOD.f   = p;             }
        inline FormatStringArg(double             p) : _fmtFun(&FormatStringArg::ff_D  ) { _valPOD.d   = p;             }
        inline FormatStringArg(long double        p) : _fmtFun(&FormatStringArg::ff_LD ) { _valPOD.ld  = p;             }
        inline FormatStringArg(bool               p) : _fmtFun(&FormatStringArg::ff_B  ) { _valPOD.b   = p;             }
        inline FormatStringArg(const void*        p) : _fmtFun(&FormatStringArg::ff_P  ) { _valPOD.p   = p;             }
        inline FormatStringArg(const char*        p) : _fmtFun(&FormatStringArg::ff_S  ) { _valStr     = Pt::String(p); }
        inline FormatStringArg(const std::string& p) : _fmtFun(&FormatStringArg::ff_S  ) { _valStr     = p.c_str();     }
        inline FormatStringArg(const Pt::String&  p) : _fmtFun(&FormatStringArg::ff_S  ) { _valStr     = p;             }

        // Formatter function entry point
        inline void operator()(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const
        { (this->*_fmtFun)(rbf, fss, numpunct); }

    private:
        // Formatter functions (one for each data type)
        void ff_I8 (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_U8 (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_I16(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_U16(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_I32(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_U32(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_I64(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_U64(Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_F  (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_D  (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_LD (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_B  (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_P  (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;
        void ff_S  (Pt::String &rbf, const FormatStringSpec& fss, const std::numpunct<Pt::Char>& numpunct) const;

    private:
        // Formatter function typedef
        typedef void (FormatStringArg::*FormatFunc)(Pt::String&, const FormatStringSpec&, const std::numpunct<Pt::Char>&) const;

        // Union for POD argument value
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

        // Argument data
        ArgValue   _valPOD;
        Pt::String _valStr;

        // Selected formatter function specific to the argument's data type
        FormatFunc _fmtFun;
};


//
// The main string-formatter class
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
#define FS_ARG_NAME_26 FS_ARG_NAME_25, FS_ARG_NAME_N(26)
#define FS_ARG_NAME_27 FS_ARG_NAME_26, FS_ARG_NAME_N(27)
#define FS_ARG_NAME_28 FS_ARG_NAME_27, FS_ARG_NAME_N(28)
#define FS_ARG_NAME_29 FS_ARG_NAME_28, FS_ARG_NAME_N(29)
#define FS_ARG_NAME_30 FS_ARG_NAME_29, FS_ARG_NAME_N(30)
#define FS_ARG_NAME_31 FS_ARG_NAME_30, FS_ARG_NAME_N(31)
#define FS_ARG_NAME_32 FS_ARG_NAME_31, FS_ARG_NAME_N(32)

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

#define FS_GENERATE_FORMAT_FUNCTION(X)                                          \
    inline Pt::String format_string(const Pt::String& format, FS_ARG_NAME_X(X)) \
    {                                                                           \
        std::vector<const FormatStringArg*> args(X);                            \
        FS_ARG_STOR_X(X);                                                       \
        const FormatString fstr(format, args);                                  \
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
FS_GENERATE_FORMAT_FUNCTION(26)
FS_GENERATE_FORMAT_FUNCTION(27)
FS_GENERATE_FORMAT_FUNCTION(28)
FS_GENERATE_FORMAT_FUNCTION(29)
FS_GENERATE_FORMAT_FUNCTION(30)
FS_GENERATE_FORMAT_FUNCTION(31)
FS_GENERATE_FORMAT_FUNCTION(32)


} // namespace


#endif

