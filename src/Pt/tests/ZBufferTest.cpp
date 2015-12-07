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
#include <Pt/ZStream.h>
#include <sstream>

class ZBufferTest : public Pt::Unit::TestSuite
{
public:
    ZBufferTest()
    : TestSuite("ZBufferTest")
    {
        Pt::Unit::TestSuite::registerMethod("DeflateInflate", 
                                            *this, &ZBufferTest::DeflateInflate);

        Pt::Unit::TestSuite::registerMethod("Stream", 
                                            *this, &ZBufferTest::Stream);

        Pt::Unit::TestSuite::registerMethod("ImportBuffer", 
                                            *this, &ZBufferTest::ImportBuffer);
    }

protected:
    void DeflateInflate()
    {
      std::stringstream oss(std::ios::in|std::ios::out|std::ios::binary);
      Pt::ZBuffer zbuf(oss);

      std::streamsize n = 0;
      for(unsigned p = 0; p < 120; ++p)
          n += zbuf.sputn("Hello World!", 12);
      
      zbuf.finish();
      std::clog << "compressed: " << oss.str().size() << " bytes." << std::endl;

      char output[2048];
      std::streamsize inflated = zbuf.sgetn( output, sizeof(output) );
      std::clog << "decompressed " << inflated << " bytes." << std::endl;
      PT_UNIT_ASSERT_EQUALS(n, inflated);
    }

    void ImportBuffer()
    {
      std::stringstream oss(std::ios::in|std::ios::out|std::ios::binary);
      Pt::ZBuffer zbuf(oss);

      std::streamsize n = 12;
      zbuf.sputn("Hello World!", 12);
      zbuf.finish();
      
      std::string data = oss.str();
      std::clog << "compressed: " << data.size() << " bytes." << std::endl;

      zbuf.detach();
      zbuf.import( data.c_str(), data.size() );

      char output[2048];
      std::streamsize inflated = zbuf.sgetn( output, sizeof(output) );
      std::clog << "decompressed " << inflated << " bytes." << std::endl;
      PT_UNIT_ASSERT_EQUALS(n, inflated);
    }

    void Stream()
    {
      std::stringstream oss(std::ios::in|std::ios::out|std::ios::binary);
      Pt::ZIOStream zstream(oss);

      std::streamsize n = 0;
      for(unsigned p = 0; p < 120; ++p)
      {
          zstream.write("Hello World!", 12);
          n += 12;
      }
      
      zstream.finish();
      std::clog << "compressed: " << oss.str().size() << " bytes." << std::endl;

      char output[2048];
      zstream.read( output, sizeof(output) );

      std::streamsize inflated = zstream.gcount();
      std::clog << "decompressed " << inflated << " bytes." << std::endl;
      PT_UNIT_ASSERT_EQUALS(n, inflated);
    }
};


Pt::Unit::RegisterTest<ZBufferTest> register_ZBufferTest;
