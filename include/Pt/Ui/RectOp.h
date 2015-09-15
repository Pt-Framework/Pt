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
#ifndef PT_UI_RECTOP_H
#define PT_UI_RECTOP_H

#include <Pt/Ui/RenderOp.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>

namespace Pt {
namespace Ui {

class RectOp : public RenderOp
{  
  public:
    RectOp( const Ui::Pen& pen, const Ui::RectF& rect)
    : _pen(pen)
    , _rect(rect)
    {
    }

    virtual void execute( Painter& painter ) const
    {
      painter.setPen( _pen );
      painter.drawRect( _rect );
    }

    
    virtual RenderOp* clone() const
    {
      return new RectOp( _pen, _rect );      
    }

  private:
     Ui::Pen    _pen;
     Ui::RectF _rect;
};

}}

#endif
