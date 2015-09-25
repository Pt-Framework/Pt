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
#ifndef PT_GFX_ELLIPSEOP_H
#define PT_GFX_ELLIPSEOP_H

#include "GraphicsOp.h"

namespace Pt {
namespace Gfx {

class EllipseOp : public GraphicsOp
{
  public:    
    
    EllipseOp(const Pen& pen, const PointF& topLeft, const SizeF& size)
    : _pen( pen ) 
    {
      points().push_back( topLeft );
    }


    EllipseOp( const EllipseOp& op )
    : GraphicsOp( op )
    , _size( op._size )
    , _pen( op._pen )
    {
    }

    virtual void execute( Painter& painter ) const 
    {
        painter.setPen( _pen );
        painter.drawEllipse( points()[0], _size );
    }

    virtual GraphicsOp* clone()  const 
    {
       return new EllipseOp( *this );
    }

  private:
    Pen _pen;
    Gfx::SizeF _size;
};

}}

#endif
