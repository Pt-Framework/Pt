 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "PaintSurfaceImpl.h"
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Ui/ImagePainter.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/RenderPath.h>
#include <Pt/Hmi/LinePath.h>
#include <Pt/Hmi/PolylinePath.h>
#include <Pt/Hmi/TextPath.h>
#include <Pt/Hmi/RectPath.h>
#include <Pt/Hmi/EllipsePath.h>
#include <Pt/Hmi/SurfacePath.h>
#include <Pt/Hmi/ImagePath.h>

namespace Pt{
namespace Hmi{

PaintSurfaceImpl::PaintSurfaceImpl()
: _image( Application::instance().impl()->frameBuffer().buffer(), 
          Ui::SizeF( 100,100),
          Ui::SizeF(Application::instance().impl()->frameBuffer().width(),  Application::instance().impl()->frameBuffer().height()),
          Application::instance().impl()->frameBuffer().strideInBytes(), 
          Application::instance().impl()->frameBuffer().format() )
, _originSize(100,100)
{	
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
}


void PaintSurfaceImpl::resize(const Ui::SizeF& size)
{
  const FrameBuffer& frameBuffer = Application::instance().impl()->frameBuffer();
	
	if( frameBuffer.depth() == 16 )
		_image.resize(size.width(), size.height(), Ui::ImageFormat::rgb565(), frameBuffer.strideInBytes() );
	else if( frameBuffer.depth() == 32 )
		_image.resize(size.width(), size.height(), Ui::ImageFormat::argb8888(), frameBuffer.strideInBytes() );

	_originSize  = size;  
}


Ui::SizeF PaintSurfaceImpl::size() const
{
	return Ui::SizeF(_image.width(), _image.height());
}


void PaintSurfaceImpl::render(Painter& painter)
{
  Ui::ImagePainter& imgPainter = painter.impl()->imagePainter();

  for( size_t i = 0; i < _path.size(); ++i )
  {
    switch( _path[i]->operation() )
    {
      case RenderPath::DrawLine:
      {
         LinePath* path = (LinePath*)_path[i];
         imgPainter.setPen( path->pen() );
         imgPainter.drawLine( path->from() , path->to() );
      }
      break;
      
      case RenderPath::DrawPolyline:
      {
         PolylinePath* path = (PolylinePath*)_path[i];
         imgPainter.setPen( path->pen() );
         imgPainter.drawPolyline( &path->points()[0], path->points().size() );
      }
      break;
      
      case RenderPath::DrawText:
      {
        TextPath* path = (TextPath*) _path[i];
        imgPainter.setPen( path->pen() );

        if( path->outline() != 0 )
          imgPainter.drawText(path->to(), path->text(), path->outline() );
        else
          imgPainter.drawText(path->to(), path->text());
      }
      break;
      
      case RenderPath::DrawRect:
      {
        RectPath* path = (RectPath*) _path[i];
        imgPainter.setPen( path->pen() );
        imgPainter.drawRect( path->rect() );
      }
      break;
      
      case RenderPath::DrawPixel:
      {
         LinePath* path = (LinePath*)_path[i];
         imgPainter.setPen( path->pen() );
         imgPainter.drawPixel( path->to() );
      }
      break;
      
      case RenderPath::DrawEllipse:
      {
        EllipsePath* path = (EllipsePath*) _path[i];
        
        imgPainter.setPen( path->pen() );
        imgPainter.drawEllipse( path->topLeft(), path->size() );
      }
      break;
      
      case RenderPath::DrawSurface:
      {//TODO:
        SurfacePath* path = (SurfacePath*) _path[i];
        path->surface().setOrigin( path->to(), path->surface().size() );
        path->surface().impl()->render( path->surface().painter() );        
      }
      break;

      case RenderPath::DrawImage:
      {
          ImagePath 
      }
      break;

      case RenderPath::FillRect:
      break;

      case RenderPath::FillEllipse:
      break;

      case RenderPath::FillPolygon:
      break;

    }
  }
}

}}
