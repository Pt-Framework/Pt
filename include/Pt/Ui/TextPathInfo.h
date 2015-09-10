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
#ifndef PT_UI_TEXTPATHINFO_H
#define PT_UI_TEXTPATHINFO_H

#include <Pt/Ui/RenderPathInfo.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Font.h>
#include <Pt/String.h>

namespace Pt {
namespace Ui {

class TextPathInfo : public RenderPathInfo
{  
  public:
    TextPathInfo( const Ui::Pen& pen, const Ui::PointF& to,  const Ui::Font& font, const Pt::String& text,  const Ui::Color* outline)
    : RenderPathInfo( RenderPathInfo::DrawText )       
    , _pen(pen)
    , _to(to)
    , _text(text)
    , _font(font)
    {
       if( outline != 0 )
        _outline = *outline;
       else
         _outline.setAlpha( -1.0f);
    }

    const Ui::Pen& pen() const
    {
      return _pen;
    }


    const Ui::PointF& to() const
    {
      return _to;
    }


    const String& text() const
    {
        return _text;
    }

    const Ui::Color* outline() const
    {
        if( _outline.alpha() == -1.0f )
            return 0;

        return &_outline;
    }

    const Ui::Font& font() const
    {
        return _font;
    }

  private:
     Ui::Pen    _pen;     
     Ui::PointF _to;
     String     _text;
     Ui::Color  _outline;  
     Ui::Font   _font;
};

}}

#endif
