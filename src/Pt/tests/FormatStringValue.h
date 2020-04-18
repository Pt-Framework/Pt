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

#ifndef PT_FORMAT_STRING_VALUE_H
#define PT_FORMAT_STRING_VALUE_H

#include <Pt/String.h>

#include "FormatStringError.h"


namespace Pt {


//
// Default values
//
#define FLOAT_LOWER_INF             "inf"
#define FLOAT_UPPER_INF             "INF"
#define FLOAT_LOWER_NAN             "nan"
#define FLOAT_UPPER_NAN             "NAN"

#define DEFAULT_TRUE_NAME           "true"
#define DEFAULT_FALSE_NAME          "false"

#define DEFAULT_DECIMAL_POINT       '.'
#define DEFAULT_THOUSANDS_SEPARATOR ','

#define DEFAULT_PRECISION           6
#define DEFAULT_GROUPING_SIZE       3


//
// Format-string value and its corresponding formatter
//
class PT_API FormatStringValue {
    public:
        // Not all systems provide locale-classes
#ifdef PT_WITH_STD_LOCALE
        typedef std::locale             locale_t;
        typedef std::numpunct<Pt::Char> numpunct_t;
#else
        struct locale_t {};
        struct numpunct_t {};
#endif

        // Formatting rule
        class Rule;

    public:
        // Constructors (one for each data type)
        inline FormatStringValue(Pt::int8_t p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_I32)
        { _valPOD.i32 = p; }

        inline FormatStringValue(Pt::uint8_t p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_I32)
        { _valPOD.u32 = p; }

        inline FormatStringValue(Pt::int16_t p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_I32)
        { _valPOD.i32 = p; }

        inline FormatStringValue(Pt::uint16_t p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_I32)
        { _valPOD.u32 = p; }

        inline FormatStringValue(Pt::int32_t p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_I32)
        { _valPOD.i32 = p; }

        inline FormatStringValue(Pt::uint32_t p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_I32)
        { _valPOD.u32 = p; }

        inline FormatStringValue(Pt::int64_t p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_I64)
        { _valPOD.i64 = p; }

        inline FormatStringValue(Pt::uint64_t p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_I64)
        { _valPOD.u64 = p; }

        inline FormatStringValue(float p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_LD)
        { _valPOD.ld = p; }

        inline FormatStringValue(double p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_LD)
        { _valPOD.ld = p; }

        inline FormatStringValue(long double p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_LD)
        { _valPOD.ld = p; }

        inline FormatStringValue(bool p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_B)
        { _valPOD.b = p; }

        inline FormatStringValue(const void* p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_P)
        { _valPOD.p = p; }

        inline FormatStringValue(char p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_C)
        { _valChr = p; }

        inline FormatStringValue(Pt::Char p)
        : _isUnsigned(true)
        , _fmtFun    (&FormatStringValue::ff_C)
        { _valChr = p; }

        inline FormatStringValue(const char* p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_S)
        { _valStr = Pt::String(p); }

        inline FormatStringValue(const std::string& p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_S)
        { _valStr = p.c_str(); }

        inline FormatStringValue(const Pt::String& p)
        : _isUnsigned(false)
        , _fmtFun    (&FormatStringValue::ff_S)
        { _valStr = p; }

        // Format value using the given rule
        inline void operator()(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
        { (this->*_fmtFun)(resBuff, rule, numpunct); }

        // Print positive floating-point number into string
        // (never calls it with a negative number, Inf, or NaN)
        static bool printPositiveFloatingPoint(Pt::String& dst, long double val, size_t precision, bool altForm, char type);

    private:
        // Formatter functions (one for each data type)
        template <typename ValueT> inline
        void ff_IXX(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;
        void ff_I32(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;
        void ff_I64(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;

        void ff_LD (Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;

        void ff_B  (Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;
        void ff_P  (Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;
        void ff_C  (Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;
        void ff_S  (Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const;

    private:
        // Formatter function typedef
        typedef void (FormatStringValue::*FormatFunc)(Pt::String&, const Rule&, const numpunct_t*) const;

        // Union for POD argument value
        union ArgValue {
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
        bool       _isUnsigned;

        ArgValue   _valPOD;
        Pt::Char   _valChr;
        Pt::String _valStr;

        // Selected formatter function specific to the argument's data type
        FormatFunc _fmtFun;

    private:
        // X-Digits
        static const char XDIGITS_LOWER[16];
        static const char XDIGITS_UPPER[16];

        static inline const char* selectXDigits(bool uppercase)
        { return uppercase ? XDIGITS_UPPER : XDIGITS_LOWER; }

        // Utilities
        template <typename ValueT>
        struct SelectInt;

        template <typename ValueT, int BASE>
        struct FormatUnsigned;
};


//
// Formatting rule
//
class FormatStringValue::Rule {
    public:
        // A constant to indicate that no precision is specified
        static const size_t NoPrecisionSpecified = (size_t) - 1;

        // Align enum
        enum Align {
            DEFAULT_ALIGN = 0,
            LEFT    = '<',
            RIGHT   = '>',
            CENTER  = '^'
        };

        // Sign enum
        enum Sign {
            DEFAULT_SIGN      = 0,
            POSITIVE_NEGATIVE = '+',
            NEGATIVE_ONLY     = '-',
            POSITIVE_SPACE    = ' '
        };

        // Type enum
        enum Type {
            DEFAULT_TYPE         = 0,
            CHARACTER            = 'c',
            STRING               = 's',
            DECIMAL              = 'd',
            BINARY_LOWER         = 'b',
            BINARY_UPPER         = 'B',
            OCTAL                = 'o',
            HEXADECIMAL_LOWER    = 'x',
            HEXADECIMAL_UPPER    = 'X',
            DECIMAL_FP           = 'f',
            HEXADECIMAL_FP_LOWER = 'a',
            HEXADECIMAL_FP_UPPER = 'A',
            SCIENTIFIC_FP_LOWER  = 'e',
            SCIENTIFIC_FP_UPPER  = 'E',
            SHORTEST_FP_LOWER    = 'g',
            SHORTEST_FP_UPPER    = 'G',
            POINTER              = 'p',
            // Below are shortcuts for the above types
            CHR        = CHARACTER,
            CHAR       = CHARACTER,
            STR        = STRING,
            DEC        = DECIMAL,
            BIN        = BINARY_LOWER,
            OCT        = OCTAL,
            HEX        = HEXADECIMAL_LOWER,
            FLT        = DECIMAL_FP,
            FLOAT      = DECIMAL_FP,
            SCIENTIFIC = SCIENTIFIC_FP_LOWER,
            PTR        = POINTER
        };

    public:
        // Constructors
        inline Rule()
        : _fill     (' '                 ) //                             default: space
        , _align    (DEFAULT_ALIGN       ) //                             default: '<' for non number and '>' for number
        , _sign     (0                   ) //                number only; default: '-'
        , _altForm  (false               ) //                number only; default: do not use the alternate form
        , _zeroPad  (false               ) //                number only; default: no use zero pad
        , _width    (0                   ) //                             default: no minimum width
        , _precision(NoPrecisionSpecified) // floating-point number only; default: not specified
        , _locale   (false               ) //                             default: no
        , _type     (0                   ) //                             default: not specified
        {}

        inline Rule(Pt::Char fill, Align align, size_t width = 0, char type = 0, bool zeroPad = false, bool altForm = false)
        : _fill     (fill                )
        , _align    (align               )
        , _sign     (0                   )
        , _altForm  (altForm             )
        , _zeroPad  (zeroPad             )
        , _width    (width               )
        , _precision(NoPrecisionSpecified)
        , _locale   (false               )
        , _type     (type                )
        {}

        inline Rule(Pt::Char fill, Align align, size_t width = 0, size_t precision = NoPrecisionSpecified, char type = 0, bool zeroPad = false, bool altForm = false)
        : _fill     (fill     )
        , _align    (align    )
        , _sign     (0        )
        , _altForm  (altForm  )
        , _zeroPad  (zeroPad  )
        , _width    (width    )
        , _precision(precision)
        , _locale   (false    )
        , _type     (type     )
        {}

        // Reset all the flags
        inline void reset()
        { *this = Rule(); }

        // Fill character (default: space)
        inline void setFill(Pt::Char fill = ' ')
        { _fill = fill; }

        inline Pt::Char fill() const
        { return _fill; }

        // Alignment (default: '<' for non number and '>' for number)
        inline void setAlign(Align align = DEFAULT_ALIGN)
        { _align = align; }

        inline Align align() const
        { return _align; }

        // Sign character (number only; default: '-')
        inline void setSign(char sign = 0)
        { _sign = sign; }

        inline char sign() const
        { return _sign; }

        // Alternate form (number only; default: do not use the alternate form)
        inline void setAltForm(bool altForm = false)
        { _altForm = altForm; }

        inline bool altForm() const
        { return _altForm; }

        // Zero pad (number only; default: no use zero pad)
        inline void setZeroPad(bool zeroPad = false)
        { _zeroPad = zeroPad; }

        inline bool zeroPad() const
        { return _zeroPad; }

        // Minimum field width (default: no minimum width)
        inline void setWidth(size_t width = 0)
        { _width = width; }

        inline size_t width() const
        { return _width; }

        // Minimum field precision (floating-point number only; default: not specified)
        inline void setPrecision(size_t precision = NoPrecisionSpecified)
        { _precision = precision; }

        inline size_t precision() const
        { return _precision; }

        // Use locale-specific formatting (default: no)
        inline void setLocale(bool locale = false)
        { _locale = locale; }

        inline bool locale() const
        { return _locale; }

        // Type (default: not specified)
        inline void setType(char type = 0)
        { _type = type; }

        inline char type() const
        { return _type; }

    private:
        Pt::Char _fill;       // fill character
        Align    _align;      // < > ^
        char     _sign;       // + - [space]
        bool     _altForm;    // #
        bool     _zeroPad;    // 0 (if the 0 character and an align option both appear, the 0 character is ignored)
        size_t   _width;      // minimum field width
        size_t   _precision;  // floating-point precision
        bool     _locale;     // use locale-specific formatting
        char     _type;       // none/s b B c d o x X a A e E f/F g G p
};



} // namespace


#endif

