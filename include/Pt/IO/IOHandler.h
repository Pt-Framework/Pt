/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
 **************************************************************************/

#ifndef Pt_IOHandler_h
#define Pt_IOHandler_h

#include <Pt/IO/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/IO/IODevice.h>
#include <Pt/IO/Url.h>

namespace Pt {

namespace IO {

    class IOHandler;


    class PT_IO_API IOTask : public NonCopyable {
        public:
            IOTask(IOHandler& handler, const Url& url)
            : _handler(handler), _url(url)
            {}

            virtual ~IOTask()
            {}

            //! Returns a pointer to the IOHandler
            inline IOHandler& handler() const throw()
            { return _handler; }
    
            //! Returns the Url addressed with this request
            inline const Url& url() const throw()
            { return _url; }

        private:
            IOHandler& _handler;
            Url _url;
    };


    class PT_IO_API GetTask : public IOTask {
        public:
            GetTask(IOHandler& handler, const Url& url)
            : IOTask(handler, url)
            {}

            virtual ~GetTask()
            {}

            virtual IODevice& open() throw (IOError) = 0;
    };


    class PT_IO_API PutTask : public IOTask {
        public:
            PutTask(IOHandler& handler, const Url& url)
            : IOTask(handler, url)
            {}

            virtual ~PutTask()
            {}

            virtual IODevice& open() throw (IOError) = 0;
    };


    class PT_IO_API UnlinkTask : public IOTask {
        public:
            UnlinkTask(IOHandler& handler, const Url& url)
            : IOTask(handler, url)
            {}

            virtual ~UnlinkTask()
            {}

            virtual void unlink() throw (IOError) = 0;
    };


    class PT_IO_API MakeDirTask : public IOTask {
        public:
            MakeDirTask(IOHandler& handler, const Url& url)
            : IOTask(handler, url)
            {}

            virtual ~MakeDirTask()
            {}

            virtual void mkdir() throw (IOError) = 0;
    };


    class PT_IO_API RemoveDirTask: public IOTask {
        public:
            RemoveDirTask(IOHandler& handler, const Url& url)
            : IOTask(handler, url)
            {}

            virtual ~RemoveDirTask()
            {}

            virtual void rmdir() throw (IOError) = 0;
    };


    class PT_IO_API ListDirTask : public IOTask {
        public:
            ListDirTask(IOHandler& handler, const Url& url)
            : IOTask(handler, url)
            {}

            virtual ~ListDirTask()
            {}
    
            virtual void list() throw (IOError) = 0;
    };


    //! Base class to handle protocol specific IO.
    class PT_IO_API IOHandler {
        public:
            IOHandler()
            {}

            virtual ~IOHandler()
            {}

            virtual GetTask* get(const Url& url) throw (IOError)
            { throw IOError("IO operation not supported.", PT_SOURCEINFO); }

            virtual PutTask* put(const Url& url) throw (IOError)
            { throw IOError("IO operation not supported.", PT_SOURCEINFO); }

            virtual UnlinkTask* unlink(const Url& url)
            { throw IOError("IO operation not supported.", PT_SOURCEINFO); }

            virtual MakeDirTask* mkdir(const Url& url) throw (IOError)
            { throw IOError("IO operation not supported.", PT_SOURCEINFO); }

            virtual RemoveDirTask* rmdir(const Url& url) throw (IOError)
            { throw IOError("IO operation not supported.", PT_SOURCEINFO); }

            virtual ListDirTask* list(const Url& url) throw (IOError)
            { throw IOError("IO operation not supported..", PT_SOURCEINFO); }
    };

} // namespace IO

} // namespace Pt

#endif
