/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA */
#include "MainWindow.h"
#include "Dialog1.h"
#include <Pt/System/Clock.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/ImageReader.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ClipPolygon.h>
#include <Pt/Gfx/Rasterizer.h>
#include "DemoImage.h"
#include "AtesionIcon.h"
#include <sstream>
#include <fstream>

namespace Pt{
namespace Hmi{
namespace Demo{

MainWindow::MainWindow()
: _child1()
, _child2()
{	
	setPosition (  Gfx::PointF(0,0) );
	setSize( Gfx::SizeF(600,400) );
    setTitle("Main 1");
	_child1.setPosition ( Gfx::PointF( 200, 200 ) );
	_child1.setTitle( "Child 1" );
	_child2.setTitle( "Child 2" );

	add( _child1 );
	add( _child2 );

}

MainWindow::~MainWindow()
{
}


void MainWindow::show()
{
  setVisible( true );	
  invalidate();	
}

}}}
