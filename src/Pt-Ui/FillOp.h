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
#ifndef PT_UI_FILLOP_H
#define PT_UI_FILLOP_H

#include "RenderOp.h"

namespace Pt {
namespace Ui {

class FillOp : public RenderOp
{
  public:    
    
    FillOp(const Brush& brush, const PointF* points, size_t count )
    : _brush( brush ) 
    {
      outline().assign( points, points + count );
    }

    FillOp(const Brush& brush, const RectF& rect )
    : _brush( brush ) 
    {
      outline().push_back( rect.topLeft() );
      outline().push_back( rect.topRight() );
      outline().push_back( rect.bottomRight() );
      outline().push_back( rect.bottomLeft() );      
    }

    FillOp( const FillOp& op )
    : RenderOp( op )
    , _brush( op._brush )
    {
    }

    virtual void execute( Painter& painter ) const 
    {
        painter.setBrush( _brush );
        painter.fillPolygon( &outline()[0], outline().size() );
    }

    virtual RenderOp* clone()  const 
    {
       return new FillOp( *this );
    }

    virtual std::string name() const
    {
      return "fill";
    }

  private:
    Brush _brush;
};

}}

#endif
