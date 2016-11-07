/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {

namespace Hmi {

PushButton::PushButton()
{
    
}


PushButton::~PushButton()
{
}


void PushButton::onPressed()
{
    Base::onPressed();

    setPressed(true);
}


void PushButton::onReleased()
{
    Base::onReleased();

    setPressed(false);
    clicked().send(*this);
}


void PushButton::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{    
    Application& app = Application::instance();
    
    const Style& s = style() ? *style() 
                             : app.style();
    
    const StyleOptions& so = styleOptions() ? *styleOptions() 
                                            : app.styleOptions();

    const ButtonRenderer* renderer = s.get<ButtonRenderer>();
    if(renderer)
        renderer->renderBackground(*this, so, surface, updateRect);
}


void PushButton::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Application& app = Application::instance();

    const Style& s = style() ? *style() 
                             : app.style();
    
    const StyleOptions& so = styleOptions() ? *styleOptions() 
                                            : app.styleOptions();

    const ButtonRenderer* renderer = s.get<ButtonRenderer>();
    if(renderer)
        renderer->renderContent(*this, so, surface, updateRect);
}

} // namespace

} // namespace
