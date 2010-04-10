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
#include "regexp.h"

class RegExpTest : public Pt::Unit::TestSuite
{
    public:
        RegExpTest()
        : Pt::Unit::TestSuite("RegExpTest")
        {
            Pt::Unit::TestSuite::registerMethod( "BTest", *this, &RegExpTest::Test );
            Pt::Unit::TestSuite::registerMethod( "ATest", *this, &RegExpTest::Test2 );
            //Pt::Unit::TestSuite::registerMethod( "MatchNumber", *this, &RegExpTest::MatchNumber );
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
        try
        {
            //Pt::String expr = L"^([0-9]+) (abc)?";
            Pt::String expr = L"([0-9]+)";
            const Pt::Char* cexpr = expr.c_str();

            std::clog << "1 compile " << cexpr  << std::endl;
            regexp* exp = regcomp( const_cast<Pt::Char*>(cexpr) );
            if( ! exp )
                return;
            std::clog << "1 compile done" << std::endl;

            Pt::String str = L"123 abc";
            const Pt::Char* cstr = str.c_str();
            int r =  regexec( exp, const_cast<Pt::Char*>(cstr) );

            std::cerr << "Result: " << r << std::endl;
            for(int n = 0; n < 10 && exp->startp[n] ; ++n)
            {
                std::cerr << n <<": " << exp->startp[n] - cstr  << std::endl;
                std::cerr << n <<": " << exp->endp[n] - cstr << std::endl;
            }

            //::free(exp);
         } catch(const std::exception& ex)
         {
            std::clog << "EX: " << ex.what() << std::endl;
         }
         catch(...)
         {
            std::clog << "EX: unknown"<< std::endl;
         }
            std::exit(1);
        }

        void Test2()
        {
            Pt::String expr = L"([0-9]+)";
            const Pt::Char* cexpr = expr.c_str();
            std::clog << "2 compile" << std::endl;
            regexp* exp = regcomp( const_cast<Pt::Char*>(cexpr) );
            std::clog << "2 compile done" << std::endl;

            Pt::String str = L"123 abc";
            const Pt::Char* cstr = str.c_str();
            std::clog << "compile" << std::endl;
            int r =  regexec( exp, const_cast<Pt::Char*>(cstr) );
            std::clog << "compile done" << std::endl;

            std::cerr << "Result: " << r << std::endl;
            for(int n = 0; n < 10 && exp->startp[n] ; ++n)
            {
                std::cerr << n <<": " << exp->startp[n] - cstr  << std::endl;
                std::cerr << n <<": " << exp->endp[n] - cstr << std::endl;
            }

            //::free(exp);
            //std::exit(1);
        }

        // void MatchNumber()
        // {
        //     std::clog << "matching number" << std::endl;
        //     Pt::String expr = L"([0-9]+)";
        //     Pt::String str = L"123 abc";

        //     Pt::RegexSMatch smatch;

        //     std::clog << "compile" << std::endl;
        //     Pt::Regex regex(expr);

        //     std::clog << "match" << std::endl;
        //     regex.match(str, smatch);

        //     std::clog << "matches: " << smatch.size() << std::endl;
        //     std::exit(1);
        // }
};

Pt::Unit::RegisterTest<RegExpTest> register_RegExpTest;

