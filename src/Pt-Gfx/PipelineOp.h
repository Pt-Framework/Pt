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
#ifndef PT_GFX_PIPELINEOP_H
#define PT_GFX_PIPELINEOP_H

#include "GraphicsOp.h"
#include <Pt/Gfx/GraphicsPipeline.h>

namespace Pt {
namespace Gfx {

class PipelineOp : public GraphicsOp
{
  public:        
    PipelineOp( const RectF& clip, const GraphicsPipeline& ops )    
    : _ops( ops )
		, _clip( clip )
    {
    }


    PipelineOp( const PipelineOp& op )
    : GraphicsOp( op )    
    , _ops( op._ops )
		, _clip( op._clip )
    {       
    }


    virtual void execute( Painter& painter ) const 
    {
      RectF orgClip   = painter.clip();        			
			RectF interClip = orgClip.intersect( _clip );		 		

			if( interClip.isNull() )
				return;

			painter.setClip( interClip );
			_ops.render( painter );
			painter.setClip( orgClip );      
    }


    virtual GraphicsOp* clone()  const 
    {
       return new PipelineOp( *this );
    }

		virtual void translate( double x, double y )
		{
			_ops.translate( x, y );						
			_clip = RectF( PointF( _clip.topLeft().x() + x, _clip.topLeft().y() + y ), _clip.size() );			
		}
		
  private:
    GraphicsPipeline _ops;    
		Gfx::RectF _clip;
};

}}

#endif
