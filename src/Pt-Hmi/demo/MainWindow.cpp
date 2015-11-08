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
{	
	Position =Gfx::PointF(0,0);
	Size =Gfx::SizeF(600,400);
	ShowTitle = true;	
	ShowInTaskbar = true;
	ShowSysMenu = true;
	Border = WindowBorder::Sizeable;
	StartPostion = WindowStartPosition::CenterParent;
	
  State = Hmi::WindowState::Normal;
	StartPostion = Hmi::WindowStartPosition::CenterParent;	
	BackColor =Gfx::Color(0.5,0.5,0.5);

	_child1.Position = Gfx::PointF( 200, 200 );
	_child1.Caption = "Child 1";
	_child2.Caption = "Child 2";

	addChildWindow( _child1 );
	addChildWindow( _child2 );

}

MainWindow::~MainWindow()
{
}


void MainWindow::show()
{
	Visible = true;	
	
}

}}}
