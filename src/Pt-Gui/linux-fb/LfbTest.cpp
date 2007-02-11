/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
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
#include "PainterImpl.h"
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/System/Thread.h>

#include <iostream>


int main(int argc, char** argv)
{
    Pt::Gfx::ARgbColor white(0xffff, 0xffff, 0xffff);
    Pt::Gfx::Font font24("Vera", 24);

    Pt::Gui::PainterImpl painter;
    std::cerr << "Opened Device: " << painter.width() << "x" << painter.height() << "@" << painter.depth() << std::endl;

    for(unsigned n = 0; n < 100000; ++n)
    {
        Pt::Gfx::ARgbImage image(220, 50, Pt::Gfx::ARgbColor(n*640, 0, 0) );

        Pt::Gfx::ImagePainter imagePainter(image);
        imagePainter.setFont(font24);
        imagePainter.drawText(Pt::Math::Point(32, 33), L"Hello World!", &white);
        painter.drawImage(Pt::Math::Point(10,10), image);
        Pt::System::Thread::sleep(10);
    }

    cerr << "Done." << std::endl;
    return 0;
}
