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
#include "regexp.h"

class RegExpTest : public Pt::Unit::TestSuite
{
    public:
        RegExpTest()
        : Pt::Unit::TestSuite("RegExpTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Test", *this, &RegExpTest::Test );
        }

        virtual void setUp()
        {

        }

        virtual void tearDown()
        {

        }

    protected:
        void Test()
        {
            // char* str = "123 abc";
            // regexp* exp = regcomp("^([0-9]+) (abc)?");

            // int r =  regexec( exp, str );

            // std::cerr << "Result: " << r << std::endl;
            // for(int n = 0; n < 10 && exp->startp[n]; ++n)
            // {
            //     std::cerr << n <<": " << exp->startp[n] - str  << std::endl;
            //     std::cerr << n <<": " << exp->endp[n] - str << std::endl;
            // }

            Pt::String expr = L"^([0-9]+) (abc)?";
            const Pt::Char* cexpr = expr.c_str();
            regexp* exp = regcomp( const_cast<Pt::Char*>(cexpr) );

            Pt::String str = L"123 abc";
            const Pt::Char* cstr = str.c_str();
            int r =  regexec( exp, const_cast<Pt::Char*>(cstr) );

            std::cerr << "Result: " << r << std::endl;
            for(int n = 0; n < 10 && exp->startp[n] ; ++n)
            {
                std::cerr << n <<": " << exp->startp[n] - cstr  << std::endl;
                std::cerr << n <<": " << exp->endp[n] - cstr << std::endl;
            }

            ::free(exp);
            std::exit(1);
        }
};

Pt::Unit::RegisterTest<RegExpTest> register_RegExpTest;

