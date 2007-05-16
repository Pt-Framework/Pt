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
#include "Pt/System/IODevice.h"
#include "IOResultImpl.h"
#include <vector>
#include <map>
#include <windows.h>


namespace Pt {

namespace System {

class WakeResult : public IOResultImpl
{
    public:

        WakeResult()
        {
            HANDLE wakeHandle = CreateEvent( NULL, FALSE, FALSE, NULL );
            setHandle(wakeHandle);
        }

        virtual ~WakeResult()
        {
            CloseHandle( handle() );
        }

        virtual void onComplete()
        {
        }

        void wake()
        { SetEvent( handle() ); }    
};

class SelectorImpl
{
    public:
        SelectorImpl();

        ~SelectorImpl();

        void addDevice( IODevice& device, int waitMode );

		void complete( IOResult& result );

        void cancel( IOResult& result );

        bool wait( unsigned int msecs );

        void wake();
    
    private:        

		std::vector<IOResult*> _readers;
        WakeResult             _wakeResult;        
};

}//namespace System

}//namespace Pt

#endif
