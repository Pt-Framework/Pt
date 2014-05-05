/* 
 * Copyright (C) 2014 Marc Boris Dürner
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
        IONotifier(System::IOHandle& h)
        : _h(&h)
        , _readNotifier(h.fd, QSocketNotifier::Read)
        , _writeNotifier(h.fd, QSocketNotifier::Write)
        { 
            _readNotifier.setEnabled(false);
            connect(&_readNotifier, SIGNAL(activated(int)), this, SLOT(onRead(int)));

            _writeNotifier.setEnabled(false);
            connect(&_writeNotifier, SIGNAL(activated(int)), this, SLOT(onWrite(int)));
        }

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
            _h->events &= ~System::IOHandle::Read;
            _h->ready = System::IOHandle::Read;

            System::Selectable* s = _h->sel;
            s->run();
        }

        void onWrite(int)
        {
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

class ApplicationImpl : public QApplication
                      , public System::EventLoop 
{
    Q_OBJECT

    public:
        ApplicationImpl(int argc, char** argv);

        virtual ~ApplicationImpl();

        Pt::System::Selector& selector()
        { return _selector; }
   
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

    public slots:
        void onWakeNotify(int fd);

        void processTimers();

    private:
        QSocketNotifier _wakeNotifier;
        QTimer _masterTimer;
        System::Mutex _mutex;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;
        QtSelector _selector;
};

} // namespace

} // namespace

#endif
