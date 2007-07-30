/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mller                                 *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PTV_UNICODE_H
#define PTV_UNICODE_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Char.h>

#include <string>


namespace Pt {

namespace Unicode {

    //! @brief General category
    enum Category {
        OtherNotAssigned     = 0,     // Cn Other, Not Assigned (no characters in the file have this property)
        OtherControl         = 1,     // Cc Other, Control
        OtherFormat          = 2,     // Cf Other, Format
        OtherSurrogate       = 3,     // Cs Other, Surrogate
        OtherPrivate         = 4,     // Co Other, Private Use
        MarkNonSpacing       = 5,     // Mn Mark, Nonspacing
        MarkSpacingCombining = 6,     // Mc Mark, Spacing Combining
        MarkEnclosing        = 7,     // Me Mark, Enclosing
        SeparatorSpace       = 8,     // Zs Separator, Space
        SeparatorLine        = 9,     // Zl Separator, Line
        SeparatorParagraph   = 10,    // Zp Separator, Paragraph
        LetterUpper          = 11,    // Lu Letter, Uppercase
        LetterLower          = 12,    // Ll Letter, Lowercase
        LetterTitle          = 13,    // Lt Letter, Titlecase
        LetterModifier       = 14,    // Lm Letter, Modifier
        LetterOther          = 15,    // Lo Letter, Other
        NumberLetter         = 16,    // Nl Number, Letter
        NumberDecimal        = 17,    // Nd Number, Decimal Digit
        NumberOther          = 18,    // No Number, Other
        PunctConnector       = 19,    // Pc Punctuation, Connector
        PunctDash            = 20,    // Pd Punctuation, Dash
        PunctOpen            = 21,    // Ps Punctuation, Open
        PunctClose           = 22,    // Pe Punctuation, Close
        PunctInitial         = 23,    // Pi Punctuation, Initial quote (may behave like Ps or Pe depending on usage)
        PunctFinal           = 24,    // Pf Punctuation, Final quote (may behave like Ps or Pe depending on usage)
        PunctOther           = 25,    // Po Punctuation, Other
        SymbolMath           = 26,    // Sm Symbol, Math
        SymbolCurrency       = 27,    // Sc Symbol, Currency
        SymbolModifier       = 28,    // Sk Symbol, Modifier
        SymbolOther          = 29     // So Symbol, Other
    };

    //! @brief Bidirectional Class
    enum BidiClass {
        LeftToRight = 0,              // L
        LeftToRightEmbedding = 1,     // LRE
        LeftToRightOverride = 2,      // LRO
        RightToLeft = 3,              // R
        RightToLeftArabic = 4,        // AL
        RightToLeftEmbedding = 5,     // RLE
        RightToLeftOverride = 6,      // RLO
        PopDirectionalFormat = 7,     // PDF
        EuropeanNumber = 8,           // EN
        EuropeanNumberSeparator = 9,  // ES
        EuropeanNumberTerminator = 10, // ET
        ArabicNumber = 11,             // AN
        CommonNumberSeparator = 12,    // CS
        NonSpacingMark = 13,           // NSM
        BoundaryNeutral = 14,          // BN
        ParagraphSeparator = 15,       // B
        SegmentSeparator = 16,         // S
        Whitespace = 17,               // WS
        OtherNeutrals = 18             // ON
    };

    //! @brief Arabic Joining
    enum Joining {
        NonJoining   = 0,
        RightJoining = 1,
        DualJoining  = 2,
        JoinCausing  = 3
    };

    //! @brief Character Decomposition
    enum Decomposition {
        NoDecomp  = 0,     // Non-decomposable
        Font      = 1,     // A font variant (e.g. a blackletter form)
        NoBreak   = 2,     // A no-break version of a space or hyphen
        Initial   = 3,     // An initial presentation form (Arabic)
        Medial    = 4,     // A medial presentation form (Arabic)
        Final     = 5,     // A final presentation form (Arabic)
        Isolated  = 6,     // An isolated presentation form (Arabic)
        Encircled = 7,     // An encircled form
        Super     = 8,     // A superscript form
        Sub       = 9,     // A subscript form
        Vertical  = 10,    // A vertical layout presentation form
        Wide      = 11,    // A wide (or zenkaku) compatibility character
        Narrow    = 12,    // A narrow (or hankaku) compatibility character
        Small     = 13,    // A small variant form (CNS compatibility)
        Square    = 14,    // A CJK squared font variant
        Fraction  = 15,    // A vulgar fraction form
        Compat    = 16,    // Compatible
        Canonical = 17     // Canonical (equivalent)
    };


    //! @brief Canonical Combining
    enum Combining {
        Spacing             = 0,
        Overlays            = 1,
        Nuktas              = 7,
        VoicingMarks        = 8,
        Viramas             = 9,
        FixedStart          = 10,
        FixedEnd            = 199,
        BelowLeftAttached  = 200,     // Below left attached
        BelowAttached      = 202,     // Below attached
        BelowRightAttached = 204,     // Below right attached
        LeftAttached       = 208,     // Left attached (reordrant around single base character)
        RightAttached      = 210,     // Right attached
        AboveLeftAttached  = 212,     // Above left attached
        AboveAttached      = 214,     // Above attached
        AboveRightAttached = 216,     // Above right attached
        BelowLeft          = 218,     // Below left
        Below              = 220,     // Below
        BelowRight         = 222,     // Below right
        Left               = 224,     // Left (reordrant around single base character)
        Right              = 226,     // Right
        AboveLeft          = 228,     // Above left
        Above              = 230,     // Above
        AboveRight         = 232,     // Above right
        DoubleBelow        = 233,     // Double below
        DoubleAbove        = 234,     // Double above
        IotaSub            = 240      // Below (iota subscript)*/
    };

    //! @brief Line break
    enum LineBreak {
        BreakUnknown         = 0,          /// XX Unknown
        BreakMandatory       = 1,          /// BK Mandatory Break
        BreakReturn          = 2,          /// CR Carriage Return
        BreakLineFeed        = 3,          /// LF Line Feed
        BreakCombMark        = 4,          /// CM Attached Characters and Combining Marks
        BreakNextLine        = 5,          /// NL Next Line
        BreakSurrogate       = 6,          /// SG Surrogates
        BreakWordJoiner      = 7,          /// WJ Word Joiner
        BreakZWSpace         = 8,          /// ZW Zero Width Space
        BreakGlue            = 9,          /// GL Non-breaking Glue
        BreakContingent      = 10,         /// CB Contingent Break OpportUnity
        BreakSpace           = 11,         /// SP Space
        BreakBoth            = 12,         /// B2 Break OpportUnity Before and After
        BreakAfter           = 13,         /// BA Break OpportUnity After
        BreakBefore          = 14,         /// BB Break OpportUnity Before
        BreakHyphen          = 15,         /// HY Hyphen
        BreakOpen            = 16,         /// OP Opening Punctuation
        BreakClose           = 17,         /// CL Closing Punctuation
        BreakQuote           = 18,         /// QU Ambiguous Quotation
        BreakExclaim         = 19,         /// EX Exclamation/Interrogation
        BreakInsep           = 20,         /// IN Inseparable
        BreakNonStart        = 21,         /// NS Non Starter
        BreakInfix           = 22,         /// IS Infix Separator (Numeric)
        BreakNumeric         = 23,         /// NU Numeric
        BreakPostfix         = 24,         /// PO Postfix (Numeric)
        BreakPrefix          = 25,         /// PR Prefix (Numeric)
        BreakSymbol          = 26,         /// SY Symbols Allowing Breaks
        BreakOrdinary        = 27,         /// AL Ordinary Alphabetic and Symbol Characters
        BreakIdeograph       = 28,         /// ID Ideographic
        BreakComplex         = 29          /// SA Complex ConText (South East Asian)
    };


    //! @brief Scripts
    enum {
        Common             = 0,       /// Zyyy
        Inherited          = 1,       /// Qaai
        Latin              = 2,       /// Latn  European scripts
        Greek              = 3,       /// Grek
        Cyrillic           = 4,       /// Cyrl (Cyrs)
        Armenian           = 5,       /// Armn
        Georgian           = 6,       /// Geor (Geon, Geoa)
        Runic              = 7,       /// Runr
        Ogham              = 8,       /// Ogam
        Hebrew             = 9,       /// Hebr  Middle eastern
        Arabic             = 10,      /// Arab
        Syriac             = 11,      /// Syrc (Syrj, Syrn, Syre)
        Thaana             = 12,      /// Thaa
        Devanagari         = 13,      /// Deva  Indic
        Bengali            = 14,      /// Beng
        Gurmukhi           = 15,      /// Guru
        Gujarati           = 16,      /// Gujr
        Oriya              = 17,      /// Orya
        Tamil              = 18,      /// Taml
        Telugu             = 19,      /// Telu
        Kannada            = 20,      /// Knda
        Malayalam          = 21,      /// Mlym
        Sinhala            = 22,      /// Sinh
        Thai               = 23,      /// Thai
        Lao                = 24,      /// Laoo
        Tibetan            = 25,      /// Tibt
        Myanmar            = 26,      /// Mymr
        Khmer              = 27,      /// Khmr
        Han                = 28,      /// Hani  Asian
        Hiragana           = 29,      /// Hira
        Katakana           = 30,      /// Kana
        Hangul             = 31,      /// Hang
        Bopomofo           = 32,      /// Bopo
        Yi                 = 33,      /// Yiii
        Ethiopic           = 34,      /// Ethi  Misc
        Cherokee           = 35,      /// Cher
        CanadianAboriginal = 36,      /// Cans
        Mongolian          = 37,      /// Mong
        Gothic             = 38,      /// Goth
        Tagalog            = 39,      /// Tglg
        Hanunoo            = 40,      /// Hano
        Buhid              = 41,      /// Buhd
        Tagbanwa           = 42,      /// Tagb
        Limbu              = 43,      /// Limb
        TaiLe              = 44,      /// Tale
        Ugaritic           = 45,      /// Ugar
        Osmanya            = 46,      /// Osma
        Cypriot            = 47,      /// Cprt
        Shavian            = 48,      /// Shaw
        Deseret            = 49,      /// Dsrt
        KatakanaHiragana   = 50       /// Hrkt
    };

// Get character category
PT_API int category(const Pt::Char& ch);

inline int bidiClass(const Pt::Char& ch)
{
    return 0;
}


inline int decomposition(const Pt::Char& ch)
{
    return 0;
}


inline int combining(const Pt::Char& ch)
{
    return 0;
}


inline int isPunctuation(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case PunctConnector:
        case PunctDash:
        case PunctOpen:
        case PunctClose:
        case PunctInitial:
        case PunctFinal:
        case PunctOther:
            ret = 1;
        default:
            break;
    }

    return ret;
}


inline int isAlnum(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case LetterUpper:
        case LetterLower:
        case LetterTitle:
        case LetterModifier:
        case LetterOther:
        case NumberDecimal:
        case NumberLetter:
        case NumberOther:
            ret = 1;
        default:
            break;
    }

    return ret;
}


inline int isAlpha(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case LetterUpper:
        case LetterLower:
        case LetterTitle:
        case LetterModifier:
        case LetterOther:
            ret = 1;
        default:
            break;
    }

    return ret;
}


inline int isControl(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case MarkNonSpacing:
        case MarkSpacingCombining:
        case MarkEnclosing:
        case OtherControl:
        case OtherFormat:
        case OtherSurrogate:
        case OtherPrivate:
        case OtherNotAssigned:
            ret = 1;
        default:
            break;
    }

    return ret;
}


inline int isDigit(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case NumberDecimal:
        case NumberLetter:
        case NumberOther:
            ret = 1;
        default:
            break;
    }

    return ret;
}


inline int isGraph(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case SymbolMath:
        case SymbolCurrency:
        case SymbolModifier:
        case SymbolOther:
            ret = 1;
        default:
            break;
    }

    return ret;
}


inline int isLower(const Pt::Char& ch)
{
    return category(ch) == LetterLower ? 1 : 0;
}


inline int isPrint(const Pt::Char& ch)
{
    int ret = 1;

    switch (category(ch))
    {
        case MarkNonSpacing:
        case MarkSpacingCombining:
        case MarkEnclosing:
        case OtherControl:
        case OtherFormat:
        case OtherSurrogate:
        case OtherPrivate:
        case OtherNotAssigned:
            ret = 0;
        default:
            break;
    }

    return ret;
}


inline int isUpper(const Pt::Char& ch)
{
    return category(ch) == LetterUpper ? 1 : 0;
}


inline int isSpace(const Pt::Char& ch)
{
    int ret = 0;

    switch (category(ch))
    {
        case SeparatorSpace:
        case SeparatorLine:
        case SeparatorParagraph:
            ret = 1;
        default:
            break;
    }

    return ret;
}


PT_API Char toLower(const Pt::Char& ch);


PT_API Char toUpper(const Pt::Char& ch);


} // namespace Pt
} // namespace Unicode


#endif
