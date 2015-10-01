/*
 * Copyright (C) 2015 by Marc Boris Duerner
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

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/ZBuffer.h>
#include <sstream>

class ZBufferTest : public Pt::Unit::TestSuite
{
public:
    ZBufferTest()
    : TestSuite("ZBufferTest")
    {
        Pt::Unit::TestSuite::registerMethod("deflateInflate", 
                                            *this, &ZBufferTest::deflateInflate);
    }

protected:
    void deflateInflate()
    {
      std::stringstream oss(std::ios::in|std::ios::out|std::ios::binary);
      Pt::ZBuffer zbuf(oss);

      std::streamsize n = zbuf.sputn("Hello World!", 12);
      zbuf.finish();
      std::clog << "compressed: " << oss.str().size() << " bytes." << std::endl;
      std::string data = oss.str();

      char output[1024];
      std::streamsize inflated = zbuf.sgetn( output, sizeof(output) );
      std::clog << "decompressed " << inflated << " bytes: ";
      std::clog.write(output, inflated) << std::endl;

      oss.str(data);
      oss.clear();
      inflated = zbuf.sgetn( output, sizeof(output) );
      std::clog << "decompressed " << inflated << " bytes: ";
      std::clog.write(output, inflated) << std::endl;
    }
};


Pt::Unit::RegisterTest<ZBufferTest> register_ZBufferTest;
