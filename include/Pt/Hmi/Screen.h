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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA  02110-1301  USA
*/

#ifndef Pt_Hmi_Screen_H
#define Pt_Hmi_Screen_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <map>
#include <vector>

namespace Pt {

namespace Hmi {

class ScreenImpl;
class ApplicationImpl;

class PT_HMI_API Screen : public Visual
{
    friend class Window;

    public:
        Screen(ApplicationImpl& app);

        virtual ~Screen();

        const std::vector<Window*>& windows() const;

        std::vector<Window*>& windows();

        Window* findWindow(const std::string& name);

        double width() const;

        double height() const;
    
        Gfx::SizeF size() const;

        Gfx::PointF toUnit(const Gfx::Point& value);
      
        Gfx::SizeF toUnit(const Gfx::Size& value);
      
        Gfx::Point fromUnit(const Gfx::PointF& value);
      
        Gfx::Size fromUnit(const Gfx::SizeF& value);
      
        Gfx::Rect fromUnit(const Gfx::RectF& value);

        virtual Pt::Gfx::PointF toScreen(const Pt::Gfx::PointF& p) const
        {
            return p;
        }

        virtual Pt::Gfx::PointF fromScreen(const Pt::Gfx::PointF& p) const
        {
            return p;
        }

        double unitSizeInch() const;
      
        double unitSizeMm() const;
      
        double resolutionDPI() const;

        void update(const Gfx::RectF& updateRect);
    
        ScreenImpl* impl();

    protected:
        void onResize(Window& w, const Gfx::SizeF& s);

        void onMove(Window& w, const Gfx::PointF& p);

        void onClosing(Window& w);

        void onClose(Window& w);

        void onShow(Window& w, bool visible);

        void onActivate(Window& w);

        void onEnable(Window& w, bool enable);

        void onUpdate(Window& w, const Gfx::RectF& rect);

    protected:
        virtual void onEvent( const Event& ev );
        
        virtual void onUpdateEvent(const UpdateEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

    protected:
        void registerWindow(Window& w);

        void unregisterWindow(Window& w);
    
    private:
        ScreenImpl*          _impl;
        std::vector<Window*> _windows;
        Gfx::RectF           _updateRect;
        int                  _updates;
};

} // namespace

} // namespace

#endif
