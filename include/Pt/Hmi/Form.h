/* Copyright (C) 2015 Marc Boris Duerner
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef Pt_Hmi_Form_h
#define Pt_Hmi_Form_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class Sheet;

//
// TODO:
//  - alignment for move/resize of window
//  - WindowImpl for native and framework windows
//  - set Decorator on Window to translate positions
//
//  - enter leave events - DONE
//  - capture - DONE
//  - focus - DONE
//  - onEnter() - DONE
//  - rescaling - DONE
//  - input method - DONE
//  - mouse cursor - DONE
//  - enabled - DONE

class PT_HMI_API Form
{
    friend class Sheet;

    public:
        virtual ~Form();


        Sheet* sheet();

        const Sheet* sheet() const;

        void setSheet(Sheet* sheet);


        Widget* content();

        const Widget* content()  const;

        void setContent(Widget* widget);


        Gfx::PointF toSheet(const Sheet& sheet, 
                            const Gfx::PointF& pos) const
        { 
            return onToSheet(sheet, pos); 
        }

        Gfx::PointF fromWSheet(const Sheet& sheet, 
                               const Gfx::PointF& pos) const
        { 
            return onFromSheet(sheet, pos); 
        }

    protected:
        Form();

        virtual void onAttach(Sheet& sheet) = 0;
    
        virtual void onDetach(Sheet& sheet) = 0;

        virtual void onInit(Sheet& sheet) = 0;

        virtual void onRelease(Sheet& sheet) = 0;

        virtual Gfx::PointF onFromSheet(const Sheet& sheet, 
                                        const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onToSheet(const Sheet& sheet, 
                                      const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onToScreen(const Sheet& sheet, 
                                       const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromScreen(const Sheet& sheet, 
                                         const Gfx::PointF& pos) const = 0;

        virtual void onRepaint(Sheet& sheet, const Gfx::RectF& rect) = 0;

        virtual void onActivate(Sheet& w, bool active) = 0;

        virtual void onMove(Sheet& sheet, const Gfx::PointF& pos) = 0;

        virtual void onResize(Sheet& sheet, const Gfx::SizeF& size) = 0;

        virtual void onEnter(Sheet& sheet, Visual& v) = 0;

        virtual void onSetCapture(Sheet& sheet, Visual& target, bool capture) = 0;

        virtual bool onIsDescendantOf(const Sheet& widget, Visual& top) const = 0;

    private:
        Sheet* _sheet;
};

} // namespace

} // namespace

#endif
