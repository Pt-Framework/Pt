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
#ifndef PT_UI_STROKEOP_H
#define PT_UI_STROKEOP_H

#include "RenderOp.h"

namespace Pt {
namespace Ui {

class StrokeOp : public RenderOp
{
  public:    
    
    StrokeOp(const Pen& pen, const PointF* points, size_t count )
    : _pen( pen ) 
    {
      outline().assign( points, points + count );
    }

    StrokeOp(const Pen& pen, const PointF& from, const PointF& to )
    : _pen( pen ) 
    {
      outline().push_back( from );
      outline().push_back( to );
    }

    
    StrokeOp(const Pen& pen, const RectF& rect )
    : _pen( pen ) 
    {
      outline().push_back( rect.topLeft() );
      outline().push_back(rect.topRight() );
      outline().push_back(rect.bottomRight() );
      outline().push_back(rect.bottomLeft() );
      outline().push_back(rect.topLeft() );
    }

    StrokeOp( const StrokeOp& op )
    : RenderOp( op )
    , _pen( op._pen )
    {
    }

    virtual void execute( Painter& painter ) const 
    {
        painter.setPen( _pen );
        painter.drawPolyline( &outline()[0], outline().size() );
    }

    virtual RenderOp* clone()  const 
    {
       return new StrokeOp( *this );
    }

  private:
    Pen _pen;
};

}}

#endif
