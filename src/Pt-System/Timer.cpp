/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#include <Pt/System/Timer.h>
#include <Pt/System/Clock.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Logger.h>
#include <limits>
#include <cassert>

PT_LOG_DEFINE("Pt.System.Timer")

namespace {

inline bool checkInterval(std::size_t interval, const Pt::Timespan& now)
{
    Pt::int64_t maxRemaining = Pt::Timespan::maxMSecs();
    maxRemaining -= now.toMSecs();
    return interval > static_cast<Pt::uint64_t>(maxRemaining);
}

}

namespace Pt {

namespace System {

class Timer::Sentry
{
    public:
        Sentry(Sentry*& sentry)
        : _deleted(false)
        , _sentry(sentry)
        {
           sentry = this;
        }

        ~Sentry()
        {
            if( ! _deleted )
                this->detach();
        }

        bool operator!() const
        { return _deleted; }

        void detach()
        {
            _sentry = 0;
            _deleted = true;
        }

    bool _deleted;
    Sentry*& _sentry;
};


Timer::Timer()
: _sentry(0)
, _loop(0)
, _interval(EventLoop::WaitInfinite)
, _finished(std::numeric_limits<Pt::int64_t>::max())
, _reserved(0)
{ }


Timer::~Timer()
{
    try
    {
        this->detach();
    }
    catch(...) {}

    if(_sentry)
        _sentry->detach();
}


bool Timer::isStarted() const
{
    return _interval != EventLoop::WaitInfinite;
}


std::size_t Timer::interval() const
{
    return _interval;
}


void Timer::start(std::size_t interval)
{
    _interval = interval;
    PT_LOG_DEBUG("Timer started, interval: " << _interval);

    Timespan now = Clock::getSystemTicks();

    bool overrun = checkInterval(_interval, now);
    if(overrun)
    {
        Pt::int64_t maxTime = std::numeric_limits<Pt::int64_t>::max();
        _finished = Timespan(maxTime);
        PT_LOG_DEBUG("timer truncated to: " << _finished.toMSecs());
    }
    else
    {
        Timespan remaining( Pt::int64_t(_interval) * 1000 );
        _finished = now + remaining;
        PT_LOG_DEBUG("timer set to: " << _finished.toMSecs());
    }

    assert(_finished.toUSecs() > 0);

    if(_loop != 0)
    {
        _loop->onDetachTimer(*this);
        _loop->onAttachTimer(*this);
    }
}


void Timer::start(const Pt::Timespan& interval)
{
    // TODO: keep interval as Timespan in Timer

    start( interval.toMSecs() );
}


void Timer::stop()
{
    Pt::int64_t maxTime = std::numeric_limits<Pt::int64_t>::max();
    _finished = Timespan(maxTime);

    _interval = EventLoop::WaitInfinite;

    if(_loop != 0)
    {
        _loop->onDetachTimer(*this);
        _loop->onAttachTimer(*this);
    }
}


bool Timer::update()
{
    if( ! isStarted() )
        return false;

    Timespan now = Clock::getSystemTicks();
    return this->update(now);
}


bool Timer::update(const Timespan& now)
{
    PT_LOG_TRACE("Timer::update " << now.toUSecs() << " usecs");

    if( ! isStarted() )
        return false;

    bool hasElapsed = now >= _finished;
    PT_LOG_DEBUG("hasElapsed: " << hasElapsed);

    Timer::Sentry sentry(_sentry);

    while( isStarted() && now >= _finished )
    {
        PT_LOG_DEBUG("executing timer: " << _finished.toUSecs() << " usecs");

        bool overrun = checkInterval(_interval, now);
        if(overrun)
        {
            Pt::int64_t maxTime = std::numeric_limits<Pt::int64_t>::max();
            _finished = Timespan(maxTime);
            PT_LOG_WARN("timer truncated to: " << _finished.toMSecs());
        }
        else
        {
            Pt::int64_t intervalUSecs = Pt::int64_t(_interval) * 1000;
            std::size_t skipped = (now - _finished).toUSecs() / intervalUSecs;
            intervalUSecs += (skipped * intervalUSecs);
            _finished += Timespan(intervalUSecs);
            PT_LOG_DEBUG("timer set to: " << _finished.toMSecs());
        }

        assert(_finished.toUSecs() > 0);

        timeout().send();

        if( ! sentry )
        {
            PT_LOG_DEBUG("timer deleted, returning: " << hasElapsed);
            return hasElapsed;
        }
    }

    PT_LOG_DEBUG("Timer::update returns: " << hasElapsed);
    return hasElapsed;
}


void Timer::setActive(EventLoop& loop)
{
    if(_loop)
        throw std::logic_error("timer already active");

    loop.onAttachTimer(*this);
    _loop = &loop;
}


void Timer::detach()
{
    if(_loop)
    {
        _loop->onDetachTimer(*this);
    }

    _loop = 0;
}


#if __cplusplus >= 202002L

AsyncWait Timer::waitAsync(std::size_t ms)
{
    return AsyncWait(*this, ms);
}


AsyncWait::AsyncWait(Timer& timer, std::size_t ms)
    : _timer(timer)
    , _ms(ms)
{
}


void AsyncWait::onBegin()
{
    _timer.timeout() += slot(*this, &AsyncWait::setReady);
    _timer.start(_ms);
}


void AsyncWait::onCancel()
{
    _timer.stop();
}


void AsyncWait::await_resume()
{
    _timer.stop();
}

#endif // __cplusplus >= 202002L

} // namespace System

} // namespace Pt
