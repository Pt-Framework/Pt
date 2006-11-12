/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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

#ifndef Pt_Text_Char_h
#define Pt_Text_Char_h

#include <Pt/Api.h>
#include <Pt/Types.h>

#include <string>


namespace Pt {

namespace Text {

	class PT_EXPORT Char
	{
		public:
			//! \brief General category
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

			//! \brief Bidirectional Class
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

			//! \brief Arabic Joining
			enum Joining {
				NonJoining   = 0,
				RightJoining = 1,
				DualJoining  = 2,
				JoinCausing  = 3
			};

			//! \brief Character Decomposition
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

			//! \brief Canonical Combining
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
				BelowLeft     = 218,     // Below left
				Below         = 220,     // Below
				BelowRight    = 222,     // Below right
				Left          = 224,     // Left (reordrant around single base character)
				Right         = 226,     // Right
				AboveLeft     = 228,     // Above left
				Above         = 230,     // Above
				AboveRight    = 232,     // Above right
				DoubleBelow   = 233,     // Double below
				DoubleAbove   = 234,     // Double above
				IotaSub       = 240      // Below (iota subscript)
			};

			//! \brief Line break
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
				BreakContingent      = 10,         /// CB Contingent Break Opportunity
				BreakSpace           = 11,         /// SP Space
				BreakBoth            = 12,         /// B2 Break Opportunity Before and After
				BreakAfter           = 13,         /// BA Break Opportunity After
				BreakBefore          = 14,         /// BB Break Opportunity Before
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
				BreakComplex         = 29          /// SA Complex Context (South East Asian)
			};


			//! \brief Scripts
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

		public:
			Char()
			: _value(0)
			{}

			Char(char ch)
			: _value( (uint32_t)((unsigned char)ch) )
			{}

			Char(wchar_t ch)
			: _value(ch)
			{}

			Char(unsigned char ch)
			: _value( (uint32_t)(ch) )
			{}

			Char(const int16_t& val)
			: _value( (uint16_t)(val) )
			{}

			Char(const uint16_t& val)
			: _value(val)
			{}

			Char(const int32_t& val)
			: _value( (uint32_t)(val) )
			{}

			Char(const uint32_t& val)
			: _value(val)
			{}

			Char(const long value)
			: _value( (uint32_t)((unsigned long)value) )
			{}

			Char(const unsigned long val)
			: _value(val)
			{}

			int category() const;

			int bidiClass() const;

			int decomposition() const;

			int combining() const;

			int isAlnum() const;

			int isAlpha() const;

			int isControl() const;

			int isDigit() const;

			int isGraph() const;

			int isLower() const;

			int isPrint() const;

			int isUpper() const;

			int isPunctuation() const;

			int isSpace() const;

			Char toLower() const;

			Char toUpper() const;

			char narrow(char defaultValue) const;

		public:
			Char& operator=(wchar_t ch)
			{ _value = ch; return *this; }

			const uint32_t value() const
			{ return _value; }

			uint32_t value()
			{ return _value; }

			operator uint32_t()
			{ return _value; }

			Char& operator-=(const Char& value)
			{
				this->_value -= value._value;
				return *this;
			}

			Char& operator+=(const Char& value)
			{
				this->_value += value._value;
				return *this;
			}

			Char& operator>>=(const Char& value)
			{
				this->_value >>= value._value;
				return *this;
			}

			Char& operator<<=(const Char& value)
			{
				this->_value <<= value._value;
				return *this;
			}

			friend bool operator==(const Char& a, const Char& b)
			{ return a.value() == b.value(); }

			friend bool operator==(const Char& a, char b)
			{ return a.value() == (unsigned char)b; }

			friend bool operator==(const Char& a, int b)
			{ return a.value() == (unsigned int)b; }

			friend bool operator!=(const Char& a, const Char& b)
			{ return a.value() != b.value(); }

			friend bool operator<(const Char& a, const Char& b)
			{ return a.value() < b.value(); }

			friend bool operator>(const Char& a, const Char& b)
			{ return a.value() > b.value(); }

			friend  bool operator<=(const Char& a, const Char& b)
			{ return a.value() <= b.value(); }

			friend  bool operator>=(const Char& a, const Char& b)
			{ return a.value() >= b.value(); }

			friend Char operator+(const Char& a, const Char& b)
			{ return a.value() + b.value(); }

			friend Char operator+(const Char& a, char ch)
			{ return a.value() + ch; }

			friend Char operator-(const Char& a, const Char& b)
			{ return a.value() - b.value(); }

			friend Char operator-(const Char& a, char ch)
			{ return a.value() - ch; }

			friend Char operator|(const Char& a, const Char& b)
			{ return a.value() | b.value(); }

			friend Char operator&(const Char& a, const Char& b)
			{ return a.value() & b.value(); }

		public:
			static const Char Null;
			static const Char Replacement;
			static const Char ByteOrderMark;
			static const Char ByteOrderSwapped;

		private:
			Pt::uint32_t _value;
	};

} // namespace Text

// Pt::Text::Char is available as Pt::Char
using Text::Char;

} // namespace Pt



namespace std {

	/// @cond INTERNAL
	template<>
	struct char_traits<Pt::Text::Char>
	{
		typedef Pt::Text::Char char_type;
		typedef Pt::uint32_t int_type;
		typedef streamoff off_type;
		typedef off_t pos_type;
		typedef mbstate_t state_type;

		inline static void assign(char_type& c1, const char_type& c2)
		{
			c1 = c2;
		}

		inline static bool eq(const char_type& c1, const char_type& c2)
		{
			return c1 == c2;
		}

		inline static bool lt(const char_type& c1, const char_type& c2)
		{
			return c1 < c2;
		}

		inline static int compare(const char_type* c1, const char_type* c2, size_t n)
		{
			while(n-- > 0)
			{
				if( !eq(*s1, *s2) )
					return lt(*s1, *s2) ? -1 : +1;

				++s1;
				++s2;
			}

			return 0;
		}

		inline static size_t length(const char_type* s)
		{
			static const Pt::Char term(0);
			std::size_t n = 0;
			while( !eq(s[n], term) )
				++n;

			return n;
		}

		inline static const char_type* find(const char_type* s, size_t n, const char_type& a)
		{
			while(n-- > 0) {
				if (*s == a)
					return s;
				++s;
			}

			return 0;
		}

		inline static char_type* move(char_type* s1, const char_type* s2, int_type n)
		{
			return (Pt::Char*)memmove(s1, s2, n * sizeof(Pt::Char));
		}

		inline static char_type* copy(char_type* s1, const char_type* s2, size_t n)
		{
			return (Pt::Char*)memcpy(s1, s2, n * sizeof(Pt::Char));
		}

		inline static char_type* assign(char_type* s, size_t n, char_type a)
		{
			while(n-- > 0) {
				*(s++) = a;
			}

			return s;
		}

		inline static char_type to_char_type(const int_type& c)
		{
			return char_type(c);
		}

		inline static int_type to_int_type(const char_type& c)
		{
			return c.value();
		}

		inline static bool eq_int_type(const int_type& c1, const int_type& c2)
		{
			return c1 == c2;
		}

		inline static int_type eof()
		{
			return static_cast<char_traits<Pt::Char>::int_type>( Pt::uint32_t(-1) );
		}

		inline static int_type not_eof(const int_type& c)
		{
			return eq_int_type(c, eof()) ? 0 : c;
		}
	};

} // namespace std


#endif
