/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Sebastian Pieck                               *
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
#ifndef PT_MUTEXIMPL_H
#define PT_MUTEXIMPL_H

#include "Pt/Api.h"
#include <windows.h>

namespace Pt {

namespace System {

class MutexImpl 
{
	public:
		MutexImpl();

		MutexImpl(int recursive);

		~MutexImpl();

		void lock();

		bool tryLock();

		void unlock();

	private:
		HANDLE _handle;
};

class ReadWriteMutexImpl
{
    public:
        ReadWriteMutexImpl();

        ~ReadWriteMutexImpl();

        void readLock();

        bool tryReadLock();

        void writeLock();

        bool tryWriteLock();

        void unlock();

    private:
        void addWriter();

        void removeWriter();

	private:
        HANDLE   _mutex;
        HANDLE   _readEvent;
        HANDLE   _writeEvent;
        unsigned _readers;
        unsigned _writers;
};
	
} // namespace System

} // namespace Pt

#endif
