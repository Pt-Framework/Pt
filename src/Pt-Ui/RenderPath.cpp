 /* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
#include <Pt/Ui/RenderPath.h>
#include <Pt/Ui/ClipPolygon.h>
#include "RenderOp.h"
#include "ImageOp.h"
#include "StrokeOp.h"
#include "TextOp.h"
#include "FillOp.h"

namespace Pt {
namespace Ui {

RenderPath::RenderPath()
{
}


RenderPath::~RenderPath()
{
  clear();
}


RenderPath::RenderPath(const RenderPath& p)
{
  for( size_t i = 0; i < p.size(); ++i )
    _path.push_back( p._path[i]->clone() );
}


RenderPath& RenderPath::operator=(const RenderPath& p)
{
  clear();

  for( size_t i = 0; i < p.size(); ++i )
    _path.push_back( p._path[i]->clone() );

  return *this;
}


void RenderPath::clear()
{
  for( size_t i = 0; i < _path.size(); ++i )
      delete _path[i];

  _path.clear();
}


void RenderPath::addPath( const RenderPath& path )
{
  for( size_t i = 0; i < path.size(); ++i )
    _path.push_back( path._path[i]->clone() );
}


void RenderPath::translate( double x, double y )
{ 
  for( size_t i = 0; i < _path.size(); ++i )
  {
    for( size_t j = 0; j < _path[i]->outline().size() ; ++ j )
    {
      _path[i]->outline()[j].addX( x ) ;
      _path[i]->outline()[j].addY( y ) ;
    }
  }
}


void RenderPath::clip( const RectF& clipRect )
{
  ClipPolygon clipper;

  for( size_t i = 0; i < _path.size(); ++i )
  {
      if( _path[i]->outline().empty() )
        continue;

      clipper.clip( _path[i]->outline(), clipRect );
  }
}


void RenderPath::drawText( const Pen& pen, const Font& font, const Pt::String& text, const RectF& textRect )
{
  _path.push_back( new TextOp(pen, font, text, textRect ) );
}


void RenderPath::stroke( const Pen& pen, const PointF* points, size_t count )
{
  _path.push_back( new StrokeOp( pen, points, count ) );
}


void RenderPath::stroke( const Pen& pen, const PointF& from, const PointF& to )
{
   _path.push_back( new StrokeOp( pen, from, to ) );
}

void RenderPath::stroke( const Pen& pen, const RectF& rect )
{
  _path.push_back( new StrokeOp( pen, rect) );
}


void RenderPath::fill( const Brush& brush, const PointF* points, size_t count )
{
  _path.push_back( new FillOp(brush, points, count) );
}

void RenderPath::fill( const Brush& brush, const RectF& rect )
{
  _path.push_back( new FillOp( brush, rect ) );
}


void RenderPath::drawImage( const PointF& to, const Image& image )
{
  _path.push_back( new ImageOp(to, image) );
}


void RenderPath::render( Painter& painter ) const 
{
  for( size_t i = 0; i < _path.size(); ++i )
    _path[i]->execute( painter );
}

void RenderPath::print() const
{
  for( size_t i = 0; i < _path.size(); ++i )
    std::clog<<i<<" "<<_path[i]->name()<<std::endl;
}

}}
