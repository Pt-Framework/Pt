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
#ifndef PT_HMI_SURFACEPATH_H
#define PT_HMI_SURFACEPATH_H

#include <Pt/Ui/RenderPathInfo.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>

namespace Pt {
namespace Hmi {

class SurfacePathInfo : public Ui::RenderPathInfo
{  
  public:
    SurfacePathInfo( const PaintSurface& surface, const Ui::PointF& to, const Ui::Region* pmRegion = 0 )
    : Ui::RenderPathInfo( Ui::RenderPathInfo::DrawSurface )       
    , _surface(surface)
    , _to(to)     
    {
      if( pmRegion != 0)
        _region = *pmRegion;
      else
        _region.setLeft(-1);
    }


    const Ui::PointF& to() const
    {
      return _to;
    }


    const Ui::Region& region() const
    {
        return _region;
    }
    
    const PaintSurface& surface() const
    {
        return _surface;
    }

    PaintSurface& surface()
    {
        return _surface;
    }

  private:
     PaintSurface _surface;
     Ui::PointF _to;
     Ui::Region _region;
};

}}

#endif
