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
   MA  02110-1301  USA
*/

#ifndef Pt_Hmi_MenuItem_H
#define Pt_Hmi_MenuItem_H

#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Key.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Font.h>
#include <Pt/String.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API MenuItem : public Panel
{
    typedef Panel BaseType;

    public:
        MenuItem();
    
        virtual ~MenuItem();

        const String& text() const;

        void setText(const String& t);

        const Gfx::Image& icon() const;
        
        void setIcon(const Gfx::Image& img);

        double iconPadding() const;

        void setIconPadding(double left);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& font);

        Signal<MenuItem&>& triggered();

        Signal<MenuItem&>& removed();

    protected:
        virtual void onParentChanged(Widget* w);
        
        virtual void onClicked(const Gfx::PointF& pos);

        virtual void onShortcut(const KeyEvent& kev);

        virtual Gfx::SizeF onAutoSize() const;

        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);
        
        virtual void onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintIcon(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintItem(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintShortcut(PaintSurface& surface, const Gfx::RectF& updateRect);
  
    protected:
        virtual void onEnterEvent( const EnterEvent& ev);

        virtual void onLeaveEvent(const LeaveEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        Signal<MenuItem&> _triggered;
        Signal<MenuItem&> _removed;
        double            _iconWidth;
        Gfx::Image        _icon;
        Gfx::Font         _font;
        Pt::String        _text;
};

} // namespace

} // namespace

#endif
