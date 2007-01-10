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

	/**
	 * @brief A lightweight Unicode class (32 bits).
	 *
	 * Unicode characters are 32-bit entities. This class represents such an entity. It is lightweight, so it
	 * can be used everywhere. Most compilers treat it like an unsigned int of 32 bits.
	 *
	 * This class provides methods for testing/classification, converting to and from other formats, comparing
	 * and case-converting. To convert a character or number to a instance of this class use one of the
	 * constructors provided. To check the type of the character use one of the method starting with "is", like
	 * isLetter() or isDigit(). To compare lower- or upper-case use isUpper() and isLower(). To compare two
	 * characters the corresponding operators are overloaded accordingly. Addition and substraction is supported
	 * as well. Comparison of numeric values (>, <, ==) is supported when using these operators.
	 *
	 * The classification methods operate on the full range of Unicode characters. All methods return $true$
	 * if the character is a certain type of character. These methods are are wrappers around category() which
	 * return the Unicode-defined category of each character.
	 *
	 * Comparison is critical in Unicode as it covers the characters of the entire world where characters which
	 * look the same may be different in the thinking of numeric values (aka positions in the Unicode table)
	 * Comparing characters will compare based purely on the numeric Unicode value (code point) of the characters.
	 * Upper- and lower-casing using upper() and lower() will only work if the character has a well-defined
	 * upper/lower-case equivalent.
	 *
	 * @see Category
	 */
	class PT_API Char
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
			//! Constructs a character with a value of 0.
			Char()
			: _value(0)
			{}

			//! Constructs a character using the given 8-bit char as base for the character value.
			Char(char ch)
			: _value( (uint32_t)((unsigned char)ch) )
			{}

			//! Constructs a character using the given wide-char as base for the character value.
			Char(wchar_t ch)
			: _value(ch)
			{}

			//! Constructs a character using the given char as base for the character value.
			Char(unsigned char ch)
			: _value( (uint32_t)(ch) )
			{}

			//! Constructs a character using the given 16-bit integer as base for the character value.
			Char(const int16_t& val)
			: _value( (uint16_t)(val) )
			{}

			//! Constructs a character using the given 16-bit integer as base for the character value.
			Char(const uint16_t& val)
			: _value(val)
			{}

			//! Constructs a character using the given 32-bit integer as base for the character value.
			Char(const int32_t& val)
			: _value( (uint32_t)(val) )
			{}

			//! Constructs a character using the given 32-bit integer as base for the character value.
			Char(const uint32_t& val)
			: _value(val)
			{}

			//! Constructs a character using the given long as base for the character value.
			Char(const long value)
			: _value( (uint32_t)((unsigned long)value) )
			{}

			//! Constructs a character using the given long as base for the character value.
			Char(const unsigned long val)
			: _value(val)
			{}

			/**
			 * @brief Returns this character's category.
			 * @return This character's category.
			 * @see Category
			 */
			int category() const;

			int bidiClass() const;

			int decomposition() const;

			/**
			 * @brief Returns the combining class for this character as defined in the Unicode standard.
			 * This is mainly useful as a positioning hint for marks attached to a base character.
			 * @return The combining class for this character.
			 */
			int combining() const;

			/**
			 * @brief Determines if this character is alpha-numerical and returns $true$ if it is.
			 *
			 * An alphanumerical character is a letter or a number.
			 *
			 * @return $true$ if this character is alphanumerical and $false$ if it is not.
			 */
			int isAlnum() const;

			/**
			 * @brief Determines if this character is a letter and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a letter and $false$ if it is not.
			 */
			int isAlpha() const;

			/**
			 * @brief Determines if this character is a control character and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a contral character and $false$ if it is not.
			 */
			int isControl() const;

			/**
			 * @brief Determines if this character is a digit and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a digit and $false$ if it is not.
			 */
			int isDigit() const;

			/**
			 * @brief Determines if this character is a symbol and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a symbol and $false$ if it is not.
			 */
			int isGraph() const;

			/**
			 * @brief Determines if this character is a upper-case letter and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a upper-case and $false$ if it is not.
			 */
			int isLower() const;

			/**
			 * @brief Determines if this character is a printable character and returns $true$ if it is.
			 *
			 * Note that this gives no indication of whether the character is available in a particular font.
			 *
			 * @return $true$ if this character is printable and $false$ if it is not.
			 */
			int isPrint() const;

			/**
			 * @brief Determines if this character is a upper-case letter and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a upper-case and $false$ if it is not.
			 */
			int isUpper() const;

			/**
			 * @brief Determines if this character is a punctuation mark and returns $true$ if it is.
			 *
			 * @return $true$ if this character is a punctuation mark and $false$ if it is not.
			 */
			int isPunctuation() const;

			/**
			 * @brief Determines if this character is a whitespace character and returns $true$ if it is.
			 *
			 * Whitespace characters are spaces, tabs, line breaks, line feeds etc.
			 *
			 * @return $true$ if this character is a whitespace character and $false$ if it is not.
			 */
			int isSpace() const;

			/**
			 * @brief Returns this character as lower-case character.
			 *
			 * This character object is not changed.
			 *
			 * @return The lower-case version of this character.
			 */
			Char toLower() const;

			/**
			 * @brief Returns this character as upper-case character.
			 *
			 * This character object is not changed.
			 *
			 * @return The upper-case version of this character.
			 */
			Char toUpper() const;

			/**
			 * @brief Narrows this character into an 8-bit char if possible.
			 *
			 * If the character can not be converted into an 8-bit char because its value is
			 * greater than 255, the defaultCharacter which is passed to this method is returned.
			 *
			 * If this character is equal or lower than 255 the character is cast to char.
			 *
			 * @param The default character which is returned if this character can not be narrowed
			 * @return An 8-bit char which is a narrowed representation of this character object or
			 * the default character if this character object's value is out of range (>255).
			 */
			char narrow(char defaultValue) const;

		public:
			/**
			 * @brief Assigns the given wchar_t as new value for this character.
			 * @param ch The new wide-char value for this character.
			 * @return A reference to this object to allow concatination of operations.
			 */
			Char& operator=(wchar_t ch)
			{ _value = ch; return *this; }

			/**
			 * @brief Returns the internal value (unsigned 32 bits) of this character.
			 * @return The 32-bit-value of this character.
			 */
			uint32_t value() const
			{ return _value; }

			/**
			 * @brief This conversion operator converts the internal value of this character to unsigned 32 bits.
			 *
			 * As the internal value also is an unsigned 32-bit value, the internal value of this character
			 * ist returned.
			 *
			 * @return The character converted to unsigned 32-bit.
			 */
			operator uint32_t()
			{ return _value; }

			/**
			 * @brief Substracts the numeric value of this character and the numeric value of the given character and
			 * stores the result in this chracater class.
			 *
			 * @param value This character's numeric value is subtracted from this' character numeric value.
			 * @return A reference to this character class.
			 */
			Char& operator-=(const Char& value)
			{
				this->_value -= value._value;
				return *this;
			}

			/**
			 * @brief Sums the numeric value of this character and the numeric value of the given character and
			 * stores the result in this chracater class.
			 *
			 * @param value This character's numeric value is added to this' character numeric value.
			 * @return A reference to this character class.
			 */
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


			static const Char& null()
			{
				static const Char c(0);
				return c;
			}

			static const Char& replacement()
			{
				static const Char c(0xfffd);
				return c;
			}

			static const Char& byteOrderMark()
			{
				static const Char c(0xfeff);
				return c;
			}

			static const Char& byteOrderSwapped()
			{
				static const Char c(0xfffe);
				return c;
			}

			//! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
			//! @return $true$ if the a and b are the same character; $false$ otherwise.
			friend bool operator==(const Char& a, const Char& b)
			{ return a.value() == b.value(); }

			//! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
			//! @return $true$ if the a and b are the same character; $false$ otherwise.
			friend bool operator==(const Char& a, char b)
			{ return a.value() == (unsigned char)b; }

			//! @brief Returns $true$ if the a and b are the same character; $false$ otherwise.
			//! @return $true$ if the a and b are the same character; $false$ otherwise.
			friend bool operator==(const Char& a, int b)
			{ return a.value() == (unsigned int)b; }

			//! @brief Returns $true$ if the a and b are not the same character; $false$ otherwise.
			//! @return $true$ if the a and b are not the same character; $false$ otherwise.
			friend bool operator!=(const Char& a, const Char& b)
			{ return a.value() != b.value(); }

			//! @brief Returns $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
			//! @return $true$ if the numeric value of a is less than the numeric value of b; $false$ otherwise.
			friend bool operator<(const Char& a, const Char& b)
			{ return a.value() < b.value(); }

			//! @brief Returns $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
			//! @return $true$ if the numeric value of a is greater than the numeric value of b; $false$ otherwise.
			friend bool operator>(const Char& a, const Char& b)
			{ return a.value() > b.value(); }

			//! @brief Returns $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
			//! @return $true$ if the numeric value of a is equal or less than the numeric value of b; $false$ otherwise.
			friend  bool operator<=(const Char& a, const Char& b)
			{ return a.value() <= b.value(); }

			//! @brief Returns $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
			//! @return $true$ if the numeric value of a is equals or greater than the numeric value of b; $false$ otherwise.
			friend  bool operator>=(const Char& a, const Char& b)
			{ return a.value() >= b.value(); }

			//! @brief Sums the numeric value of a and the numeric value of b and returns the sum.
			//! @return The sum of the numeric values of a and b.
			friend Char operator+(const Char& a, const Char& b)
			{ return a.value() + b.value(); }

			//! @brief Sums the numeric value of a and the numeric value of b and returns the sum.
			//! @return The sum of the numeric values of a and b.
			friend Char operator+(const Char& a, char ch)
			{ return a.value() + ch; }

			//! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
			//! @return The substraction of the numeric values of b from a.
			friend Char operator-(const Char& a, const Char& b)
			{ return a.value() - b.value(); }

			//! @brief Subtracts the numeric value of b from the numeric value of b and returns the result.
			//! @return The substraction of the numeric values of b from a.
			friend Char operator-(const Char& a, char ch)
			{ return a.value() - ch; }

			//! @brief Does an OR-combination of the numeric value of a and b and returns the result.
			//! @return The OR-combination of the numeric values of a and b.
			friend Char operator|(const Char& a, const Char& b)
			{ return a.value() | b.value(); }

			//! @brief Does an AND-combination of the numeric value of a and b and returns the result.
			//! @return The AND-combination of the numeric values of a and b.
			friend Char operator&(const Char& a, const Char& b)
			{ return a.value() & b.value(); }

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
		typedef streampos pos_type;
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

		inline static int compare(const char_type* s1, const char_type* s2, size_t n)
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
