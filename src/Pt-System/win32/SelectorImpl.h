/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#ifndef PT_SYSTEM_SelectorImpl_H
#define PT_SYSTEM_SelectorImpl_H

#include "Pt/System/Api.h"
#include <vector>
#include <map>
#include <windows.h>


namespace Pt {

namespace System {

class IOChannel;

class SelectorImpl
{
    public:
        SelectorImpl();

        ~SelectorImpl();
        
        void addChannel( IOChannel& channel );

        void removeChannel( IOChannel& channel );

        bool wait( unsigned int msecs );

        void wake(); 

    private:
        void collectWaitHandles(std::vector<HANDLE>& waitHandles);

        bool areNonWaitableDevicesAvailable();
    
        void sendEvents(const HANDLE activeHandle);

    private:     
        enum{ InternalWake = 0 };
        
        std::vector<IOChannel*>          _channels;
        std::map<HANDLE, IOChannel*>    _channelMap;
        HANDLE                          _wakeHandle;   
};

}//namespace System 

}//namespace Pt

#endif
