/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
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
#ifndef PT_SYSTEM_RWMUTEX_H
#define PT_SYSTEM_RWMUTEX_H

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace System {

/// A reader writer lock allows multiple concurrent
/// readers or one exclusive writer.
class PT_SYSTEM_API RWMutex : public NonCopyable
{
    public:
        RWMutex();
            /// Creates the Reader/Writer lock.

        ~RWMutex();
            /// Destroys the Reader/Writer lock.

        void readLock();
            /// Acquires a read lock. If another thread currently holds a write lock,
            /// waits until the write lock is released.

        bool tryReadLock();
            /// Tries to acquire a read lock. Immediately returns true if successful, or
            /// false if another thread currently holds a write lock.

        void writeLock();
            /// Acquires a write lock. If one or more other threads currently hold 
            /// locks, waits until all locks are released. The results are undefined
            /// if the same thread already holds a read or write lock

        bool tryWriteLock();
            /// Tries to acquire a write lock. Immediately returns true if successful,
            /// or false if one or more other threads currently hold 
            /// locks. The result is undefined if the same thread already
            /// holds a read or write lock.

        void unlock();
            /// Releases the read or write lock.

    private:
        class RWMutexImpl* _impl;
};


class ReadLock
{
    public:
    	ReadLock(RWMutex& m, bool doLock = true)
    	: _mutex(m)
    	, _locked(false)
    	{
      		if(doLock)
        		this->lock();
		}

    	~ReadLock()
    	{
    		try 
    		{
     			if(_locked)
        			this->unlock();
        	}
        	catch(...)
        	{}
   		}

   		void lock()
    	{
    		if( ! _locked )
       		{
      			_mutex.readLock();
        		_locked = true;
			}
    	}

    	void unlock()     
    	{
    		if( _locked)
        	{
      			_mutex.unlock();
        		_locked = false;
			}
    	}

	private:
    	RWMutex& _mutex;
    	bool _locked;
};


class WriteLock
{
    public:
    	WriteLock(RWMutex& m, bool doLock = true)
    	: _mutex(m)
    	, _locked(false)
    	{
      		if(doLock)
        		this->lock();
		}

    	~WriteLock()
    	{
    		try 
    		{
     			if(_locked)
        			this->unlock();
        	}
        	catch(...)
        	{}
   		}

   		void lock()
    	{
    		if( ! _locked )
       		{
      			_mutex.writeLock();
        		_locked = true;
			}
    	}

    	void unlock()     
    	{
    		if( _locked)
        	{
      			_mutex.unlock();
        		_locked = false;
			}
    	}

	private:
    	RWMutex& _mutex;
    	bool _locked;
};

} // !namespace System

} // !namespace Pt

#endif
