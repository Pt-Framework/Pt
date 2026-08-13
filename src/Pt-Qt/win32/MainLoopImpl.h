/*
 * Copyright (C) 2014 Marc Boris Duerner
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

#include <win32/Selector.h>
#include <Pt/Qt/Api.h>
#include <Pt/System/EventLoop.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QWinEventNotifier>
#include <QtCore/QTimer>
#include <vector>

namespace Pt {

namespace Qt {

class MainLoopImpl : public QObject
{
    Q_OBJECT

    public:
        MainLoopImpl(QCoreApplication& app, Signal<const Pt::Event&>& ev);

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
        void onOverlapped(HANDLE h);

        void onWake(HANDLE h);

        void processTimers();

    private:
        QCoreApplication& _app;
        Signal<const Event&>& _event;
        QTimer _masterTimer;
        System::Mutex _mutex;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;
        Pt::System::Selector _selector;
        QWinEventNotifier _overlappedNotifier;
        QWinEventNotifier _wakeNotifier;
};

} // namespace

} // namespace

#endif
