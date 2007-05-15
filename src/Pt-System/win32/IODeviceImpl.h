/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef PT_SYSTEM_IODEVICEIMPL_H
#define PT_SYSTEM_IODEVICEIMPL_H

#include "Pt/System/IODevice.h"

#include <windows.h>
#include <vector>


namespace Pt {

namespace System {


	class IODeviceImpl
	{
		public:
			IODeviceImpl();

			virtual ~IODeviceImpl();			

			virtual HANDLE deviceHandle() const  = 0;			
	};


    class IOResultImpl : public IOResult
    {
        public:
            virtual IOResultImpl* impl()
            { return this; }

            void setHandle(HANDLE h)
            { _handle = h; }

            HANDLE handle() const
            { return _handle; }

			virtual void onComplete() = 0;

		protected:
            IOResultImpl()
            : IOResult()
            , _handle(INVALID_HANDLE_VALUE)
            {}
 
		private:
            HANDLE _handle;
    };

	class ReadResult : public IOResultImpl
	{
        public:
            ReadResult()
            {}

			virtual void onComplete()
			{
				this->device()->inputReady();
			}
	};


	class WriteResult : public IOResultImpl
	{
        public:
            WriteResult()
            {}

			virtual void onComplete()
			{
				this->device()->outputReady();
			}
	};

} //namespace

} //namespace

#endif
