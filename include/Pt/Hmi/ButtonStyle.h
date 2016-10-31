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

#ifndef Pt_Hmi_ButtonStyle_H
#define Pt_Hmi_ButtonStyle_H

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>

namespace Pt {

namespace Hmi {

class Button;
class PaintSurface;

class WidgetStyle
{
    public:
        WidgetStyle(const std::type_info& ti)
        : _typeId(&ti)
        {}
    
        virtual ~WidgetStyle()
        {}

        const std::type_info& typeId() const
        {
            return *_typeId;
        }

    private:
        const std::type_info* _typeId;
};


class PT_HMI_API ButtonStyle : public WidgetStyle
{
    public:
        ButtonStyle();
    
        virtual ~ButtonStyle();

        void renderBackground(Button& button, 
                              PaintSurface& surface, 
                              const Gfx::RectF& updateRect) const
        { 
            onRenderBackground(button, surface, updateRect); 
        }    

        void renderContent(Button& button, 
                           PaintSurface& surface, 
                           const Gfx::RectF& updateRect) const
        { 
            onRenderContent(button, surface, updateRect); 
        }   

        const Pt::Gfx::Pen& foreground() const
        { 
            return _pen; 
        }
        
        void setPen(const Pt::Gfx::Pen& pen)
        { 
            _pen = pen; 
        }

        void setBackground(const Pt::Gfx::Brush&  b)
        { 
            _brush = b;
        }

        const Pt::Gfx::Brush& background() const
        { 
            return _brush; 
        }

        const Pt::Gfx::Font& font() const
        { 
            return _font; 
        }

    protected:
        virtual void onRenderBackground(Button& button, 
                                        PaintSurface& surface, 
                                        const Gfx::RectF& updateRect) const = 0;

        virtual void onRenderContent(Button& button, 
                                     PaintSurface& surface, 
                                     const Gfx::RectF& updateRect) const = 0;

    private:
        Pt::Gfx::Pen   _pen;
        Pt::Gfx::Brush _brush;
        Pt::Gfx::Font  _font;        
};


class PT_HMI_API PtButtonStyle : public ButtonStyle
{
    public:
        PtButtonStyle();
    
        virtual ~PtButtonStyle();

    protected:
        virtual void onRenderBackground(Button& button, 
                                        PaintSurface& surface, 
                                        const Gfx::RectF& updateRect) const;
        
        virtual void onRenderContent(Button& button, 
                                     PaintSurface& surface, 
                                     const Gfx::RectF& updateRect) const;  


    private:
        Gfx::Color _borderColor;
};

} // namespace

} // namespace

#endif
