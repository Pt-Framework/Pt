/* Copyright (C) 2016 Marc Boris Duerner 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/PngWriter.h>

#include <fstream>

class PtGfxTest : public Pt::Unit::TestSuite
{
    public:
        PtGfxTest()
        : Pt::Unit::TestSuite("Pt-Gfx-test")
        {
            //using namespace Pt;

            //Gfx::ImagePainter2::setDefaultFont("DejaVu Sans");
        
            //Gfx::Image image( Gfx::ImageFormat::argb32(), Gfx::Size(600, 600) );
            //Gfx::ImagePainter2 imagePainter(image);

            //Pt::Gfx::Transform trans;    
            //trans.rotateDeg(90);

            //imagePainter.setFont( Pt::Gfx::Font("", 32) );
            //
            //imagePainter.setPen( Gfx::Color::fromRgb8(255, 0, 0) );
            //imagePainter.drawText(Pt::Gfx::PointF(200, 200), "rotated", trans);

            //imagePainter.setPen( Gfx::Color::fromRgb8(0, 255, 0) );
            //imagePainter.drawText(Pt::Gfx::PointF(200, 50), "not rotated");

            //imagePainter.setPen( Gfx::Color::fromRgb8(255, 255, 0) );
            //imagePainter.drawLine( Gfx::PointF(0,0), Gfx::PointF(200, 200) );

            //std::clog << "PtGfxTest: writing gfx-test-image.png" << std::endl;
            //std::ofstream ofs("gfx-test-image.png", std::ios::out|std::ios::trunc);
            //Gfx::PngWriter pngWriter(ofs);
            //pngWriter.write(image);
            //ofs.close();
        }
};

Pt::Unit::RegisterTest<PtGfxTest> _registerGfxTest;
