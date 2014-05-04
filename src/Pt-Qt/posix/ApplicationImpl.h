/* Copyright (C) 2014 Marc Boris Dürner
 * 
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_Qt_ApplicationImpl_h
#define Pt_Qt_ApplicationImpl_h

#include <posix/Selector.h>
#include <Pt/Qt/Api.h>
#include <Pt/System/EventLoop.h>
#include <QtWidgets/QApplication>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>
#include <vector>

namespace Pt {

namespace Qt {

class QtSelector : public QObject
                 , public System::Selector
{
    Q_OBJECT

    public:
        SelectorImpl()
        : _current(0)
        { }

        ~SelectorImpl()
        {         
            while( ! _selectables.empty() )
            {
                _selectables.first()->detach();
            }
        }

        void attach(Selectable& s)
        {
            _selectables.insert(s);
        }
        
        void detach(Selectable& s)
        {
            SelectableList::unlink(s);
        }

        void cancel(IOHandle& h)
        {
            IOMap::iterator it = _iomap.find(&h);
            if( it != _iomap.end() )
            {
                delete it->second;
                _iomap.erase(it);
            }
        }

        void beginRead(IOHandle* h)
        {
            QSocketNotifier* notifier = new QSocketSotifier(h->fd, QSocketNotifier::Read);
            _iomap[h] = notifier;

            // connect to slot which calls run
        }

        void endRead(IOHandle* h)
        {
            IOMap::iterator it = _iomap.find(h);
            if( it != _iomap.end() )
            {
                delete it->second;
                _iomap.erase(it);
            }
        }

        void beginWrite(IOHandle* h)
        {
            enableSelect(h);
            FD_SET(h->fd, &_wfds);
        }

        void endWrite(IOHandle* h)
        {
            FD_CLR( h->fd, &_wfds );
        }

        bool isReadable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_rfdsOut);
        }

        bool isWritable(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_wfdsOut);
        }

        bool isError(IOHandle* h)
        {
            return FD_ISSET(h->fd, &_efdsOut);
        }

        void wake()
        {
            _wakePipe.wake();
        }

        bool isWoken()
        { _wakePipe.isReady(); }

        typedef std::map<IOHandle*, QSocketNotifier*> IOMap;

    private:
        WakePipe _wakePipe;
        SelectableList _selectables;
        IOMap _iomap;
        Selectable* _current;
};

class ApplicationImpl : public QApplication
                      , public System::EventLoop 
{
    Q_OBJECT

    public:
        ApplicationImpl(int argc, char** argv);

        virtual ~ApplicationImpl();

        Pt::System::Selector& selector()
        { return _selector; }

    public slots:
        void onOverlapped(HANDLE h);

        void onWake(int fd);

        void processTimers();
   
    protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onCancel(System::Selectable& s);

        virtual void onReady(System::Selectable& s);

        virtual void onRun();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& ev);

        virtual void onQueueEvent(const Pt::Event& ev);

        virtual void onWake();

        virtual void onAttachTimer(System::Timer& timer);

        virtual void onDetachTimer(System::Timer& timer);

    private:
        QtSelector _selector;
        QWinEventNotifier _overlappedNotifier;
        QsocketNotifier _wakeNotifier;
        QTimer _masterTimer;
        System::Mutex _mutex;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;
};

} // namespace

} // namespace

#endif
