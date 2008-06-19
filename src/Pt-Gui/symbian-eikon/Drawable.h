/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PT_GUI_SYMBIAN_DRAWABLE_H
#define PT_GUI_SYMBIAN_DRAWABLE_H

#include <Pt/Api.h>
#include "PainterImpl.h"

namespace Pt {

namespace Gui {

    class Drawable
    {
        public:
            virtual ~Drawable()
            { }

            /** 
             * @brief Enable drawing to native graphics context.
             * This will be called by the painer to retrieve context information.
             */ 
            virtual PainterImpl::ContextInfo beginDraw() = 0;            
            
            /**
             * @brief Disable drawing to native gfx context.
             * This will be called by the painer to end drawing.
             */
            virtual void endDraw() = 0;            
    };

} // namespace Gui

} // namespace Pt

#endif
