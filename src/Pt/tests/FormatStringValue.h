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

    public:
        // Formatting rule
        struct Rule {
            // Member flags
            Pt::Char fill;       // fill character
            char     align;      // < > ^
            char     sign;       // + - [space]
            bool     altForm;    // #
            bool     zeroPad;    // 0 (if the 0 character and an align option both appear, the 0 character is ignored)
            size_t   width;      // minimum field width (default 0)
            size_t   precision;  // floating-point precision (default 6)
            bool     locale;     // use locale-specific formatting
            char     type;       // none/s b B c d o x X a A e E f/F g G p

            // Constant to indicate that no precision is specified
            static const size_t NoPrecisionSpecified = (size_t) - 1;

            // Constructor
            inline Rule()
            { reset(); }

            inline Rule(Pt::Char fill_, char align_, size_t width_ = 0, char type_ = 0, bool zeroPad_ = false, bool altForm_ = false)
            {
                reset();

                fill    = fill_;
                align   = align_;
                altForm = altForm_;
                zeroPad = zeroPad_;
                width   = width_;
                type    = type_;
            }

            inline Rule(Pt::Char fill_, char align_, size_t width_ = 0, size_t precision_ = NoPrecisionSpecified, char type_ = 0, bool zeroPad_ = false, bool altForm_ = false)
            {
                reset();

                fill      = fill_;
                align     = align_;
                altForm   = altForm_;
                zeroPad   = zeroPad_;
                width     = width_;
                precision = precision_;
                type      = type_;
            }

            // Reset all the flags
            inline void reset()
            {
                fill      = ' ';                  // default: space
                align     = 0;                    // default: '<' for non number and '>' for number
                sign      = 0;                    // default: '-' for number
                altForm   = false;                // default: no alternate form
                zeroPad   = false;                // default: no zero pad
                width     = 0;                    // default: no minimum width
                precision = NoPrecisionSpecified; // default: no precision specified
                locale    = false;                // default: do not use locale-specific formatting
                type      = 0;                    // default: none
            }
        };

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

        // Format positive floating-point number
        static bool formatPositiveFP(Pt::String& dst, long double val, size_t precision, bool altForm, char type);

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


} // namespace


#endif

