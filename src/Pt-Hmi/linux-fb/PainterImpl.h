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
#ifndef PT_HMI_PAINTERIMPL_H
#define PT_HMI_PAINTERIMPL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/RenderPath.h>
#include <Pt/Ui/ImagePainter.h>
#include <Pt/Ui/FontMetrics.h>

namespace Pt {
namespace Hmi {

class PaintSurface;



class PainterImpl
{
  public:
    PainterImpl(PaintSurfaceImpl* surface);
		
    virtual ~PainterImpl();
  
		void setRenderMode(Ui::RenderMode::Type mode)
		{
		}
	 
		virtual void setPen(const Ui::Pen& pen);

    virtual const Ui::Pen& pen() const;

    virtual void setBrush(const Ui::Brush& brush);

    virtual const Ui::Brush& brush() const;

    virtual void setFont(const Ui::Font& font);

    virtual const Ui::Font& font() const;

    virtual Ui::FontMetrics fontMetrics() const;

    virtual Ui::FontMetrics fontMetrics(Pt::String Text) const;

    virtual const std::list<std::string>& fontFamilyNames();

    virtual int depth() const;


    virtual void drawPixel(const Ui::PointF& to);

    virtual void drawLine(const Ui::PointF& from, const Ui::PointF& to);
    
    virtual void drawPolyline(const Ui::PointF* points, const size_t pointCount);

	  virtual void drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline );

    virtual void drawText(const Ui::PointF& to, const Pt::String& Text);

    virtual void drawRect(const Ui::RectF& rectangle);

    virtual void drawEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size);

    virtual void drawSurface(const Ui::PointF& to, PaintSurface& pm, const Ui::Region& pmRegion);

    virtual void drawSurface(const Ui::PointF& to, PaintSurface& pm);
		
    virtual void drawImage(const Ui::PointF& to, const Ui::Image& image);

    virtual void drawImage(const Ui::PointF& to, const Ui::Image& image, const Ui::Region& imageRegion);    


    virtual void fillRect(const Ui::RectF& rectangle);    

    virtual void fillEllipse(const Ui::PointF& topLeft, const Ui::SizeF& size);

    virtual void fillPolygon(const Ui::PointF* points, const size_t pointCount);

    virtual void addFontName(const std::string& fontName);

	  void setSurface( PaintSurface& s );	    
		
		void flush();

  private:
    Ui::PointF fromOrigin(const Ui::PointF& p);

    Ui::RectF fromOrigin(const Ui::RectF& p);

  private:
	  Ui::ImagePainter  _painter;
	  PaintSurfaceImpl*  _surface;	
    Ui::Pen           _pen;
    Ui::Brush         _brush;
    Ui::Font          _font;
};

}}

#endif
