/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan

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

class PT_HMI_API MenuItem  : public Panel
{
    public:
        MenuItem();
    
        virtual ~MenuItem();

        const String& text() const 
        {
            return _text;
        }

        void setText(const String& t);

        const Gfx::Image& icon() const
        {
            return _icon;
        }
        
        void setIcon(const Gfx::Image& img);

        const Gfx::Font& font() const
        {
            return _font;
        }

        void setFont(const Gfx::Font& font);

        const Key& shortcut() const
        {
            return _shortcut;
        }

        void setShortcut(const Key& k);

        Pt::Signal<MenuItem&>& contentChanged()
        {
            return _contentChanged;
        }

    protected:    
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onResizeEvent(const ResizeEvent& ev);

    protected: 
        virtual Gfx::SizeF onAutoSize() const;

    private:
        Gfx::Image  _icon;
        Gfx::Font   _font;
        Pt::String  _text;
        Key         _shortcut;
        Pt::Signal<MenuItem&> _contentChanged;
};

} // namespace

} // namespace

#endif
