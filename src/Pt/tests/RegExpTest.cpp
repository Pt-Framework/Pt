/*
 * Copyright (C) 2008-2010 by Dr. Marc Boris Duerner
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
#undef PT_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Regex.h"

class RegExpTest : public Pt::Unit::TestSuite
{
    public:
        RegExpTest()
        : Pt::Unit::TestSuite("RegExpTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Alphabetic", *this, &RegExpTest::Alphabetic );
            Pt::Unit::TestSuite::registerMethod( "Numbers", *this, &RegExpTest::Numbers );
            Pt::Unit::TestSuite::registerMethod( "MissingBrace", *this, &RegExpTest::MissingBrace );
        }

    protected:
        void Alphabetic()
        {
            Pt::String expr = L"([a-z]+) ([A-Z]+) (xyz)";
            Pt::String str = L"abc DEF xyz ...";

            Pt::RegexSMatch smatch;
            Pt::Regex regex(expr);
            regex.match(str, smatch);

            //std::clog << "matches: " << smatch.size() << std::endl;
            PT_UNIT_ASSERT_EQUALS(smatch.size(), 4);

            PT_UNIT_ASSERT_EQUALS(smatch.offsetBegin(0), 0);
            PT_UNIT_ASSERT_EQUALS(smatch.offsetEnd(0), 11);
            PT_UNIT_ASSERT(smatch.get(0) == L"abc DEF xyz");

            PT_UNIT_ASSERT_EQUALS(smatch.offsetBegin(1), 0);
            PT_UNIT_ASSERT_EQUALS(smatch.offsetEnd(1), 3);
            PT_UNIT_ASSERT(smatch.get(1) == L"abc");

            PT_UNIT_ASSERT_EQUALS(smatch.offsetBegin(2), 4);
            PT_UNIT_ASSERT_EQUALS(smatch.offsetEnd(2), 7);
            PT_UNIT_ASSERT(smatch.get(2) == L"DEF");

            PT_UNIT_ASSERT_EQUALS(smatch.offsetBegin(3), 8);
            PT_UNIT_ASSERT_EQUALS(smatch.offsetEnd(3), 11);
            PT_UNIT_ASSERT(smatch.get(3) == L"xyz");

            // for(unsigned n = 0; n < smatch.size(); ++n)
            // {
            //     std::clog << "match " << n << " ("
            //               << smatch.offsetBegin(n) << " - "
            //               << smatch.offsetEnd(n) << "): '"
            //               << smatch.get(n).narrow() << "'" << std::endl;
            // }
        }

        void Numbers()
        {
            Pt::String expr = L"([0-9]+) ([0-9]+) ([7-9]+) (000)";
            Pt::String str = L"123 456 789 000";

            Pt::RegexSMatch smatch;
            Pt::Regex regex(expr);
            regex.match(str, smatch);

            PT_UNIT_ASSERT(smatch.size() == 5);
        }

        void MissingBrace()
        {
            Pt::String expr = L"([0-9]+";
            Pt::String str = L"123";

            Pt::RegexSMatch smatch;

            PT_UNIT_ASSERT_THROW(Pt::Regex regex(expr), Pt::InvalidRegex);
        }
};

Pt::Unit::RegisterTest<RegExpTest> register_RegExpTest;

