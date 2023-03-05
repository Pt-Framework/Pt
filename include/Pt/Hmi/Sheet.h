/* Copyright (C) 2022 Marc Boris Duerner
  
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

#ifndef PT_HMI_SHEET_H
#define PT_HMI_SHEET_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Hmi {

class Form;

class PT_HMI_API Sheet : public Visual
{
    friend class Form;

    public:
        virtual ~Sheet();

        Gfx::PointF toForm(const Form& form, 
                           const Gfx::PointF& pos) const
        { 
            return onToForm(form, pos); 
        }

        Gfx::PointF fromForm(const Form& form, 
                             const Gfx::PointF& pos) const
        { 
            return onFromForm(form, pos); 
        }

    protected:
        Sheet();

        virtual void onAttach(Form& form) = 0;
    
        virtual void onDetach(Form& form) = 0;

        virtual void onInit(Form& form) = 0;

        virtual void onRelease(Form& form) = 0;

        virtual Gfx::PointF onFromForm(const Form& form, 
                                       const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onToForm(const Form& form, 
                                     const Gfx::PointF& pos) const = 0;

        virtual void onRepaintRequest(Form& form, const Gfx::RectF& rect) = 0;

        virtual void onActivateRequest(Form& form, bool active) = 0;

        virtual void onEnableRequest(Form& form, bool isEnable) = 0;

        virtual void onShowRequest(Form& form, bool isShow) = 0;

        virtual void onMoveRequest(Form& form, const Gfx::PointF& pos) = 0;

        virtual void onResizeRequest(Form& form, const Gfx::SizeF& size) = 0;
};

} // namespace

} // namespace

#endif
