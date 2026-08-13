/*
 * Copyright (C) 2014 Marc Boris Dürner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 * 02110-1301, USA.
 */

#ifndef Pt_Qt_MainLoopImpl_h
#define Pt_Qt_MainLoopImpl_h

#include <posix/Selector.h>
#include <posix/../SelectableList.h>
#include <Pt/Qt/Api.h>
#include <Pt/System/EventLoop.h>
#include <QtWidgets/QApplication>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>
#include <vector>

namespace Pt {

namespace Qt {

class IONotifier : public QObject
{
    Q_OBJECT

    public:
        IONotifier(System::IOHandle& h);

        void enableRead()
        { _readNotifier.setEnabled(true); }

        void disableRead()
        { _readNotifier.setEnabled(false); }

        void enableWrite()
        { _writeNotifier.setEnabled(true); }

        void disableWrite()
        { _writeNotifier.setEnabled(false); }

    public slots:
        void onRead(int)
        {
            disableRead();

            _h->events &= ~System::IOHandle::Read;
            _h->ready = System::IOHandle::Read;

            System::Selectable* s = _h->sel;
            s->run();
        }

        void onWrite(int)
        {
            disableWrite();

            _h->events &= ~System::IOHandle::Write;
            _h->ready = System::IOHandle::Write;

            System::Selectable* s = _h->sel;
            s->run();
        }

    private:
        System::IOHandle* _h;
        QSocketNotifier _readNotifier;
        QSocketNotifier _writeNotifier;
};


class QtSelector : public QObject
                 , public System::Selector
{
    Q_OBJECT

    public:
        QtSelector()
        : _current(0)
        { }

        ~QtSelector()
        {
            while( ! _selectables.empty() )
            {
                _selectables.first()->detach();
            }
        }

        void attach(System::Selectable& s)
        {
            _selectables.insert(s);
        }
        
        void detach(System::Selectable& s)
        {
            System::SelectableList::unlink(s);
        }

        void cancel(System::IOHandle& h)
        {
            if(h.id == System::IOHandle::InvalidId)
                return;

            IOMap::iterator it = _iomap.find(&h);
            if( it != _iomap.end() )
            {
                delete it->second;
                _iomap.erase(it);
            }

            h.id = System::IOHandle::InvalidId;
            h.ready = 0;
            h.events = 0;
        }
        
        IONotifier& getNotifier(System::IOHandle* h)
        {
            if(h->id == System::IOHandle::InvalidId)
            {
                IONotifier* notifier = new IONotifier(*h);
                _iomap[h] = notifier;
                h->id = 1;
                return *notifier;
            }

            return *_iomap[h];
        }
        
        void beginRead(System::IOHandle* h)
        {
            IONotifier& notifier = getNotifier(h);

            notifier.enableRead();
            h->events = System::IOHandle::Read;
        }

        void endRead(System::IOHandle* h)
        {
            if(h->events & System::IOHandle::Read)
            {
                 IONotifier& notifier = getNotifier(h);
                 notifier.disableRead();
            }

            h->ready = 0;
            h->events &= ~System::IOHandle::Read;
        }

        void beginWrite(System::IOHandle* h)
        {
            IONotifier& notifier = getNotifier(h);

            notifier.enableWrite();
            h->events = System::IOHandle::Write;
        }

        void endWrite(System::IOHandle* h)
        {
            if(h->events & System::IOHandle::Write)
            {
                 IONotifier& notifier = getNotifier(h);
                 notifier.disableWrite();
            }

            h->ready = 0;
            h->events &= ~System::IOHandle::Write;
        }

        bool isReadable(System::IOHandle* h)
        {
            bool isReady = h->ready == System::IOHandle::Read;
            return isReady;
        }

        bool isWritable(System::IOHandle* h)
        {
            bool isReady = h->ready == System::IOHandle::Write;
            return isReady;
        }

        bool isError(System::IOHandle* h)
        {
            return false;
        }

        void wake()
        {
            _wakePipe.wake();
        }

        bool isWoken()
        { return _wakePipe.isReady(); }

        int wakeFd()
        { return _wakePipe.readFd(); }

        typedef std::map<System::IOHandle*, IONotifier*> IOMap;

    private:
        System::WakePipe _wakePipe;
        System::SelectableList _selectables;
        IOMap _iomap;
        System::Selectable* _current;
};

class MainLoopImpl : public QObject
{
    Q_OBJECT

    public:
        MainLoopImpl(QCoreApplication& app, Signal<const Pt::Event&>& eventSignal);

        virtual ~MainLoopImpl();

        Pt::System::Selector& selector()
        { return _selector; }

        virtual void attachSelectable(System::Selectable&);

        virtual void detachSelectable(System::Selectable&);

        virtual void cancel(System::Selectable& s);

        virtual void ready(System::Selectable& s);

        virtual void run();

        virtual void exit();

        virtual void commitEvent(const Pt::Event& ev);

        virtual void queueEvent(const Pt::Event& ev);

        virtual void processEvents();

        virtual void wake();

        virtual void attachTimer(System::Timer& timer);

        virtual void detachTimer(System::Timer& timer);

    public slots:
        void onWakeNotify(int fd);

        void processTimers();

    private:
        QCoreApplication& _app;
        Signal<const Event&>& _event;
        QTimer _masterTimer;
        System::Mutex _mutex;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;
        QtSelector _selector;
        QSocketNotifier _wakeNotifier;
};

} // namespace

} // namespace

#endif
