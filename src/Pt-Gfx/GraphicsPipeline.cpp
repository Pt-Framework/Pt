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
#include <Pt/Gfx/GraphicsPipeline.h>
#include "GraphicsOp.h"
#include "ImageOp.h"
#include "StrokeOp.h"
#include "TextOp.h"
#include "PipelineOp.h"
#include "FillOp.h"
#include "EllipseOp.h"
#include "FillEllipseOp.h"

namespace Pt {
namespace Gfx {

GraphicsPipeline::GraphicsPipeline()
{
}


GraphicsPipeline::~GraphicsPipeline()
{
  clear();
}


GraphicsPipeline::GraphicsPipeline(const GraphicsPipeline& p)
{
  for( size_t i = 0; i < p.size(); ++i )
    _path.push_back( p._path[i]->clone() );
}


GraphicsPipeline& GraphicsPipeline::operator=(const GraphicsPipeline& p)
{
  clear();

  for( size_t i = 0; i < p.size(); ++i )
    _path.push_back( p._path[i]->clone() );

  return *this;
}


void GraphicsPipeline::clear()
{
  for( size_t i = 0; i < _path.size(); ++i )
      delete _path[i];

  _path.clear();
}


void GraphicsPipeline::addPipeline( const RectF& clip,  const GraphicsPipeline& path )
{
    _path.push_back( new PipelineOp(clip, path) );
}


void GraphicsPipeline::translate( double x, double y )
{ 
  for( size_t i = 0; i < _path.size(); ++i )
		_path[i]->translate( x, y );
}


void GraphicsPipeline::text( const PointF& to, const Pen& pen, const Font& font, const Pt::String& text)
{
  _path.push_back( new TextOp(to, pen, font, text) );
}


void GraphicsPipeline::stroke( const Pen& pen, const PointF* points, size_t count )
{
  _path.push_back( new StrokeOp( pen, points, count ) );
}


void GraphicsPipeline::stroke( const Pen& pen, const PointF& from, const PointF& to )
{
   _path.push_back( new StrokeOp( pen, from, to ) );
}

void GraphicsPipeline::stroke( const Pen& pen, const RectF& rect )
{
  _path.push_back( new StrokeOp( pen, rect) );
}


void GraphicsPipeline::fill( const Brush& brush, const PointF* points, size_t count )
{
  _path.push_back( new FillOp(brush, points, count) );
}

void GraphicsPipeline::fill( const Brush& brush, const RectF& rect )
{
  _path.push_back( new FillOp( brush, rect ) );
}


void GraphicsPipeline::ellipse( const Pen& pen, const PointF& leftTop, const Gfx::SizeF& size )
{
  _path.push_back( new EllipseOp( pen, leftTop, size ) );
}


void GraphicsPipeline::fillEllipse( const Brush& brush, const PointF& leftTop, const Gfx::SizeF& size )
{
  _path.push_back( new FillEllipseOp( brush, leftTop, size ) );
}


void GraphicsPipeline::image( const PointF& to, const Image& image )
{
  _path.push_back( new ImageOp(to, image) );
}


void GraphicsPipeline::render( Painter& painter ) const 
{
  for( size_t i = 0; i < _path.size(); ++i )
    _path[i]->execute( painter );
}

}}
