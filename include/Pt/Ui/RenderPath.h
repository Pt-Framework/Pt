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
#ifndef PT_UI_RENDERPATH_H
#define PT_UI_RENDERPATH_H

#include <Pt/Ui/Api.h>
#include <Pt/Types.h>
#include <Pt/String.h>
#include <Pt/Ui/Painter.h>
#include <vector>

namespace Pt {
namespace Ui {

class RenderOp;

class PT_UI_API RenderPath
{  
  public:
    RenderPath();

    RenderPath(const RenderPath& p);

    RenderPath& operator=(const RenderPath& p);

    virtual ~RenderPath();
  
    void clear();

    size_t size() const
    {
      return _path.size();
    }

    void addPath( const RenderPath& path );   

    void drawText( const Pen& pen, const Font& font, const PointF& to, const Pt::String& text, const RectF& textRect ); 
   
    void stroke( const Pen& pen, const PointF* points, size_t count );
    
    void stroke( const Pen& pen, const PointF& from, const PointF& to );

    void stroke( const Pen& pen, const RectF& rect );

    void fill( const Brush& brush, const PointF* points, size_t count );
    
    void fill( const Brush& brush, const RectF& rect );

    void drawImage( const PointF& to, const Image& image );

    void translate( double x, double y );
   
    void clip( const RectF& clipRect );

    void render( Painter& painter ) const;

  private:
    std::vector<RenderOp*> _path;      
};

}}

#endif
