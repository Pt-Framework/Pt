/*
 * Copyright (C) 2015 Marc Boris Duerner
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

#include "Pt/Api.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/WindowsCodec.h"
#include "Pt/TextStream.h"
#include "Pt/StringStream.h"
#include <string>
#include <sstream>
#include <cstring>

char Text1252[]  = { (char)0x80, (char)0xba, (char)0xe1, (char)0xbd, (char)0xb9, (char)0xcf, (char)0x0 };

class WindowsCodecTest : public Pt::Unit::TestSuite
{
    public:
        WindowsCodecTest()
        : Pt::Unit::TestSuite("WindowsCodecTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Windows1252Decode",
                                                 *this, &WindowsCodecTest::Windows1252Decode );

        }

        void Windows1252Decode()
        {
            std::istringstream iss(Text1252);
            Pt::TextIStream tis(iss, new Pt::Windows1252Codec);

            Pt::String result;
            std::getline(tis, result);

            PT_UNIT_ASSERT_EQUALS(result.size(), 6);
            PT_UNIT_ASSERT_EQUALS(result[0], 0x20ac);
        }
};


Pt::Unit::RegisterTest<WindowsCodecTest> _registerWindowsCodecTest;
