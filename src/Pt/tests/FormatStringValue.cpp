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

// References:
//     https://en.cppreference.com/w/cpp/utility/format/format
//     https://en.cppreference.com/w/cpp/utility/format/formatter#Standard_format_specification
//     https://en.cppreference.com/w/cpp/chrono/system_clock/formatter#Format_specification
//
// Results verified using:
//     https://fmt.dev/latest/index.html
//     https://github.com/fmtlib/fmt/releases/tag/6.2.0
//     https://github.com/fmtlib/fmt/releases/tag/4.1.0

#include <cmath>

#include "FormatStringValue.h"

#include "FormatStringValue_InlineInternal.h"


namespace Pt {

//
// Format-string value and its corresponding formatter
//
const char FormatStringValue::XDIGITS_LOWER[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

const char FormatStringValue::XDIGITS_UPPER[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


template <typename ValueT> inline
void FormatStringValue::ff_IXX(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{
    // TODO: Optimize!

    // Preparation
    typedef typename SelectInt<ValueT>::SignedT   SignedT;
    typedef typename SelectInt<ValueT>::UnsignedT UnsignedT;

    bool      negNum;
    UnsignedT numVal;

    if(_isUnsigned) {
        negNum = false;
        numVal = SelectInt<ValueT>::selectUnsigned(this);
    }
    else {
        const SignedT sigVal = SelectInt<ValueT>::selectSigned(this);

        negNum = (sigVal < 0);
        numVal = negNum ? -sigVal : sigVal;
    }

    // For backward compatibility with older (draft) C++20 standard
    char ruleType   = rule.type;
    bool ruleLocale = rule.locale;

    if(ruleType == 'n') {
        ruleLocale = true;
        ruleType   = 'd';
    }

    // Handle 'sign' as a prefix character
    Pt::String prefixStr;

    if(!rule.sign || rule.sign == '-') {
        if(negNum) {
            prefixStr = '-';
        }
    }
    else if(rule.sign == '+') {
        if(negNum) prefixStr = '-';
        else       prefixStr = '+';
    }
    else if(rule.sign == ' ') {
        if(negNum) prefixStr = '-';
        else       prefixStr = ' ';
    }
    else {
        throw FormatStringError("invalid 'sign specifier' in format string");
    }

    // Determine the thousands separator and grouping size
    Pt::Char           thousandsSep    = 0;
    size_t             groupingSize    = 0;
    const Pt::uint8_t* groupingSizePtr = 0;
    size_t             groupingSizeCnt = 0;

    if(ruleLocale) {

#ifdef PT_WITH_STD_LOCALE
        // Get the locale-specific thousands separator and grouping size if possible
        if(numpunct) {
            thousandsSep = numpunct->thousands_sep();
            if(numpunct->grouping().length() > 1) {
                groupingSizePtr = (const Pt::uint8_t*) &numpunct->grouping()[0];
                groupingSizeCnt =                       numpunct->grouping().length();
            }
            else {
                groupingSize = numpunct->grouping()[0];
            }
        }
#endif
        // Otherwise, use the default thousands separator and grouping size
        if(!thousandsSep) thousandsSep = DEFAULT_THOUSANDS_SEPARATOR;
        if(!groupingSize) groupingSize = DEFAULT_GROUPING_SIZE;
    }

    // Temporary result buffers
    Pt::String strVal;
    Pt::String tmpResBuff;

    // Process as base 2 type
    if( TYPE_IS_B(ruleType) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            prefixStr += '0';
            prefixStr += ruleType;
        }
        // Convert to string (in reversed direction)
        FormatUnsigned<UnsignedT, 2>::printReversed(strVal, numVal);
        // Reverse the string and add thousands separator as needed
        if(groupingSizePtr)
            FormatUnsigned<UnsignedT, 2>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSizePtr, groupingSizeCnt);
        else
            FormatUnsigned<UnsignedT, 2>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSize);
    }
    // Process as base 8 type
    else if( TYPE_IS_O(ruleType) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            prefixStr += '0';
        }
        // Convert to string (in reversed direction)
        FormatUnsigned<UnsignedT, 8>::printReversed(strVal, numVal);
        // Reverse the string and add thousands separator as needed
        if(groupingSizePtr)
            FormatUnsigned<UnsignedT, 8>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSizePtr, groupingSizeCnt);
        else
            FormatUnsigned<UnsignedT, 8>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSize);
    }
    // Process as base 16 type
    else if( TYPE_IS_X(ruleType) ) {
        // Handle '#' as prefix characters
        if(rule.altForm) {
            prefixStr += '0';
            prefixStr += ruleType;
        }
        // Convert to string (in reversed direction)
        FormatUnsigned<UnsignedT, 16>::printReversed(strVal, numVal, ruleType == 'X');
        // Reverse the string and add thousands separator as needed
        if(groupingSizePtr)
            FormatUnsigned<UnsignedT, 16>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSizePtr, groupingSizeCnt);
        else
            FormatUnsigned<UnsignedT, 16>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSize);
    }
    // Process as base 10 type
    else if( TYPE_IS_D(ruleType) ) {
        // Alternate form cannot be used with base 10 type
        if(rule.altForm)
            throw FormatStringError("format specifier '#' requires 'b/B/o/x/X' numeric argument");
        // Convert to string (in reversed direction)
        FormatUnsigned<UnsignedT, 10>::printReversed(strVal, numVal);
        // Reverse the string and add thousands separator as needed
#ifdef PT_WITH_STD_LOCALE
        //if(ruleLocale && (groupingSize != 3 || groupingSizeCnt))
        //    throw FormatStringError("only locale with default grouping ('\\3') is supported for decimal");
#endif
        if(groupingSizePtr)
            FormatUnsigned<UnsignedT, 10>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSizePtr, groupingSizeCnt);
        else
            FormatUnsigned<UnsignedT, 10>::reverseAndGroupString(tmpResBuff, strVal, thousandsSep, groupingSize);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }

    // Put the prefix characters
    size_t ruleWidth = rule.width;

    if(rule.zeroPad) {
        if(!prefixStr.empty()) {
            if(!rule.align) {
                resBuff += prefixStr;
                ruleWidth = ( ruleWidth > prefixStr.length() ) ? ( ruleWidth - prefixStr.length() ) : 0;
            }
            else {
                tmpResBuff = prefixStr + tmpResBuff;
            }
        }
    }
    else if(!prefixStr.empty()) {
        tmpResBuff = prefixStr + tmpResBuff;
    }

    // Handle '0'
    const Pt::Char& ruleFill = (rule.zeroPad && !rule.align) ? Pt::Char('0') : rule.fill;

    // The default alignment for numeric argument is right
    const char ruleAlign = rule.align ? rule.align : '>';

    // Process the generated string
    Rule r(ruleFill, ruleAlign, ruleWidth, 's');
    FormatStringValue(tmpResBuff).ff_S(resBuff, r, numpunct);
}


void FormatStringValue::ff_I32(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{ ff_IXX<Pt::int32_t>(resBuff, rule, numpunct); }


void FormatStringValue::ff_I64(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{ ff_IXX<Pt::int64_t>(resBuff, rule, numpunct); }


void FormatStringValue::ff_LD(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{
    // TODO: Optimize!

    // Preparation
    const bool        negNum = (_valPOD.ld < 0.0L);
    const long double numVal = negNum ? -_valPOD.ld : _valPOD.ld;

    // Temporary result buffer
    Pt::String tmpResBuff;

    // Process Inf and NaN
    if(isinf(numVal)) {
        if(Pt::isupper(rule.type)) tmpResBuff = FLOAT_UPPER_INF;
        else                       tmpResBuff = FLOAT_LOWER_INF;
    }
    else if(isnan(numVal)) {
        if(Pt::isupper(rule.type)) tmpResBuff = FLOAT_UPPER_NAN;
        else                       tmpResBuff = FLOAT_LOWER_NAN;
    }

    // Handle 'sign' as a prefix character
    Pt::String prefixStr;

    if(!rule.sign || rule.sign == '-') {
        if(negNum) {
            prefixStr = '-';
        }
    }
    else if(rule.sign == '+') {
        if(negNum) prefixStr = '-';
        else       prefixStr = '+';
    }
    else if(rule.sign == ' ') {
        if(negNum) prefixStr = '-';
        else       prefixStr = ' ';
    }
    else {
        throw FormatStringError("invalid 'sign specifier' in format string");
    }

    // Check if we have Inf or NaN
    if(!tmpResBuff.empty()) {
        // The default alignment for numeric argument is right
        const char ruleAlign = rule.align ? rule.align : '>';
        // Process as string type
        Rule r(rule.fill, ruleAlign, rule.width, 's');
        FormatStringValue(prefixStr + tmpResBuff).ff_S(resBuff, r, numpunct);
        // We are done here
        return;
    }

    // Handle 'a' and 'A'
    if( TYPE_IS_A(rule.type) ) {
        prefixStr += '0';
        prefixStr += (rule.type == 'A') ? 'X' : 'x';
    }

    // Determine the decimal point
    Pt::Char decimalPoint = 0;
    if(rule.locale) {
        // Get the locale-specific decimal point if possible
#ifdef PT_WITH_STD_LOCALE
        if(numpunct) decimalPoint = numpunct->decimal_point();
#endif
        // Otherwise, use the default decimal point
        if(!decimalPoint) decimalPoint = DEFAULT_DECIMAL_POINT;
    }

    // Determine the thousands separator
    Pt::Char thousandsSep = 0;
    if(rule.locale) {
        // Get the locale-specific thousands separator if possible
#ifdef PT_WITH_STD_LOCALE
        if(numpunct) thousandsSep = numpunct->thousands_sep();
#endif
        // Otherwise, use the default thousands separator
        if(!thousandsSep) thousandsSep = DEFAULT_THOUSANDS_SEPARATOR;
    }

    // Check the grouping size
#ifdef PT_WITH_STD_LOCALE
    if(numpunct && !numpunct->grouping().empty() && numpunct->grouping() != "\3")
        throw FormatStringError("only locale with default grouping ('\\3') is supported for floating-point");
#endif

    // Format the number
    const size_t rulePrecision = (rule.precision == Rule::NoPrecisionSpecified) ? DEFAULT_PRECISION : rule.precision;
    const char   ruleType      = rule.type ? rule.type : 'g';

    Pt::String strFP;
    printPositiveFloatingPoint(strFP, numVal, rulePrecision, rule.altForm, ruleType);

    // Format/add the decimal point and thousands separator(s) as needed
    finalizePositiveFloatingPointStringFormat(tmpResBuff, strFP, decimalPoint, thousandsSep);

    // Put the prefix characters
    size_t ruleWidth = rule.width;

    if(rule.zeroPad) {
        if(!prefixStr.empty()) {
            if(!rule.align) {
                resBuff += prefixStr;
                ruleWidth = ( ruleWidth > prefixStr.length() ) ? ( ruleWidth - prefixStr.length() ) : 0;
            }
            else {
                tmpResBuff = prefixStr + tmpResBuff;
            }
        }
    }
    else if(!prefixStr.empty()) {
        tmpResBuff = prefixStr + tmpResBuff;
    }

    // Handle '0'
    const Pt::Char& ruleFill = (rule.zeroPad && !rule.align) ? Pt::Char('0') : rule.fill;

    // The default alignment for numeric argument is right
    const char ruleAlign = rule.align ? rule.align : '>';

    // Process as string type
    Rule r(ruleFill, ruleAlign, ruleWidth, 's');
    FormatStringValue(tmpResBuff).ff_S(resBuff, r, numpunct);
}


void FormatStringValue::ff_B(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    // Process as string type
    if( TYPE_IS_S(rule.type) ) {
        // Handle '0'
        const Pt::Char& ruleFill = (rule.zeroPad && !rule.align) ? Pt::Char('0') : rule.fill;
        // Get the locale-specific string if possible
        Pt::String boolName;
#ifdef PT_WITH_STD_LOCALE
        if(rule.locale && numpunct) {
            boolName = _valPOD.b ? numpunct->truename() : numpunct->falsename();
        }
#endif
        // Otherwise, use the default string
        if(boolName.empty()) {
            boolName = _valPOD.b ? DEFAULT_TRUE_NAME : DEFAULT_FALSE_NAME;
        }
        // Process the generated string
        Rule r(ruleFill, rule.align, rule.width, rule.type);
        FormatStringValue(boolName).ff_S(resBuff, r, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_C(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, 'd', rule.zeroPad); // For boolean, type 'c' is considered the same as type 'd'
        FormatStringValue((Pt::uint32_t) _valPOD.b ? 1 : 0).ff_I32(resBuff, r, numpunct);
    }
    else if( TYPE_IS_BDOX(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, rule.type, rule.zeroPad);
        FormatStringValue((Pt::uint32_t) _valPOD.b ? 1 : 0).ff_I32(resBuff, r, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringValue::ff_P(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(rule.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(rule.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    if( !TYPE_IS_P(rule.type) )
        throw FormatStringError("invalid 'type specifier' in format string");

    // Process as numeric type
    Rule r(rule.fill, rule.align, std::max(rule.width, sizeof(void*)), 'x', false, true);
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    FormatStringValue((Pt::uint64_t) _valPOD.p).ff_I64(resBuff, r, numpunct);
#else
    FormatStringValue((Pt::uint32_t) _valPOD.p).ff_I32(resBuff, r, numpunct);
#endif
}


void FormatStringValue::ff_C(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(rule.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(rule.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    // Process as character type
    if( TYPE_IS_C(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, rule.type);
        FormatStringValue(Pt::String(1, _valChr)).ff_S(resBuff, r, numpunct);
    }
    // Process as numeric type
    else if( TYPE_IS_BDOX(rule.type) ) {
        Rule r(rule.fill, rule.align, rule.width, rule.type);
        FormatStringValue((Pt::uint32_t) _valChr).ff_I32(resBuff, r, numpunct);
    }
    // Invalid type
    else {
        throw FormatStringError("invalid 'type specifier' in format string");
    }
}


void FormatStringValue::ff_S(Pt::String& resBuff, const Rule& rule, const numpunct_t* numpunct) const
{
    // Check the specifiers
    if(rule.sign)
        throw FormatStringError("format specifier 'sign' requires numeric argument");

    if(rule.altForm)
        throw FormatStringError("format specifier '#' requires numeric argument");

    if(rule.zeroPad)
        throw FormatStringError("format specifier '0' requires numeric argument");

    if(rule.locale)
        throw FormatStringError("format specifier 'L' requires numeric/boolean argument");

    if( !TYPE_IS_S(rule.type) )
        throw FormatStringError("invalid 'type specifier' in format string");

    // The default alignment for non numeric argument is left
    const char align = rule.align ? rule.align : '<';

    // Calculate string the length
    const size_t strLen = _valStr.length();
    const size_t padLen = rule.width - strLen;

    // Resize the destination buffer
    const size_t resBuffOrgLen = resBuff.length();
    const size_t resBuffNewLen = resBuffOrgLen + std::max( strLen, rule.width );

    resBuff.resize(resBuffNewLen);

    // Get the write pointer
    Pt::Char* resBuffPtr = &resBuff[0] + resBuffOrgLen;

    // Center
    if(align == '^') {
        // Pad the string as needed
        if(strLen < rule.width) {
            // Calculate the padding sizes
            const size_t lPadLen = padLen / 2;
            const size_t rPadLen = (lPadLen * 2 == padLen) ? lPadLen : (lPadLen + 1);
            // Left pad the string
            resBuffPtr = fill(resBuffPtr, rule.fill, lPadLen);
            // Copy the string
            resBuffPtr = copy(resBuffPtr, _valStr.data(), strLen);
            // Right pad the string
            resBuffPtr = fill(resBuffPtr, rule.fill, rPadLen);
        }
        // Do not pad the string
        else {
            copy(resBuffPtr, _valStr.data(), strLen);
        }
    }
    // Right
    else if(align == '>') {
        // Left pad the string as needed
        if(strLen < rule.width) resBuffPtr = fill(resBuffPtr, rule.fill, padLen);
        // Copy the string
        copy(resBuffPtr, _valStr.data(), strLen);
    }
    // Left
    else if(align == '<') {
        // Copy the string
        resBuffPtr = copy(resBuffPtr, _valStr.data(), strLen);
        // Right pad the string as needed
        if(strLen < rule.width) fill(resBuffPtr, rule.fill, padLen);
    }
    // Invalid
    else {
        throw FormatStringError("invalid 'align specifier' in format string");
    }
}


} // namespace
