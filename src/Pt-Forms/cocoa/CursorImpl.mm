/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris D�rner
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#include "CursorImpl.h"
 #import <AppKit/NSView.h>
#import <AppKit/NSCursor.h>
#import <AppKit/NSLayoutConstraint.h>

namespace Pt {

namespace Hmi {
        
// CursorImpl::CursorImpl()
// {    
// }
        
// CursorImpl::~CursorImpl()
// {    
// }
        
// void CursorImpl::setCursor(Cursors::Type type)
// {
//     if(_type == type)  
//         return;

//     _type = type;

//     NSCursor* aCursor =  [NSCursor arrowCursor];
            
//     switch(_type)
//     {
//         default:             
//         break;
             
//         case Pt::Hmi::Cursors::Cross:
//             aCursor = [ NSCursor crosshairCursor];
//         break;
             
//         case Pt::Hmi::Cursors::Hand:
//             aCursor = [ NSCursor openHandCursor];
//         break;
             
//         case Pt::Hmi::Cursors::IBeam:
//             aCursor = [ NSCursor IBeamCursor];
//         break;
            
//         case Pt::Hmi::Cursors::SizeNS:
//             aCursor = [ NSCursor resizeUpDownCursor];
//         break;
             
//         case Pt::Hmi::Cursors::SizeWE:
//             aCursor = [ NSCursor resizeLeftRightCursor];
//         break;
//     }
    
//     //Set the cursor as current cursor.
//     [ aCursor set];
            
// }
        
// Cursors::Type CursorImpl::getCursor() const
// {
//     return _type;
// }
                
} // namespace

} // namespace
