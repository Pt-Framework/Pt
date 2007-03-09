/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Drner                                   *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
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
#ifndef PT_SYSTEM_WRITEEVENT_H
#define PT_SYSTEM_WRITEEVENT_H

#include <Pt/System/IOEvent.h>

namespace Pt{
namespace System{

class PT_SYSTEM_API WriteEvent : public IOEvent
{
    public:
        WriteEvent();
        ~WriteEvent();
        
        virtual Event* clone() const;
        virtual const std::type_info& typeInfo() const;
        
        static const std::type_info& TYPE_INFO;        
};

}//namespace System
}//namespace Pt

#endif