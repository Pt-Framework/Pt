/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2005-2007 by Sebastian Pieck                            *
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
#include "ApplicationImpl.h"
#include "WidgetImpl.h"
#include "PainterImpl.h"
#include <Pt.h>

#include <iostream>

int main(int argc, char** argv)
{
	Pt::Gui::ApplicationImpl app;
	Pt::Gui::WidgetImpl widget;
	//	  PgSetStrokeColor(Pg_BLUE);
  //  PgDrawILine(1,2,10,21);
	widget.move(0, 500);
	widget.resize(400, 50);
	widget.show();

	Pt::Gui::WidgetImpl widget2;
	widget2.show();
 //PgSetStrokeColor(Pg_BLUE);
 
 
	Pt::Gui::PainterImpl& painter = widget2.painter();
	//painter.setRid( widget2.rid() );
	painter.begin();
	painter.setPen( Pt::Gfx::Pen(Pt::Gfx::ARgbColor(0xffff, 0x0000, 0x0000) ) );
	painter.drawLine( Pt::Math::Point(10,10), Pt::Math::Point(100,200) );
	painter.end();
	
	//PdSetTargetDevice( PhDCGetCurrent(), widget.rid() );
	
		/*PgSetRegion( widget.rid() );
	  PgSetStrokeColor(Pg_BLUE);
    PgDrawILine(1,2,100,101);
    PtFlush();

		//PgSetRegion( widget2.rid() );
    	PgSetStrokeColor(Pg_GREEN);
    PgDrawILine(1,5,100,105);
    PtFlush();*/
    
	return app.run();
}

#/** PhEDIT attribute block
#-11:16777215
#0:1619:default:-3:-3:0
#1619:1635:monospace9:0:-1:0
#1635:1755:default:-3:-3:0
#1755:1813:monospace9:0:-1:0
#1813:1925:default:-3:-3:0
#1925:1958:monospace9:0:-1:0
#1958:2324:default:-3:-3:0
#2324:2517:monospace9:0:-1:0
#2517:2539:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000337)**/
