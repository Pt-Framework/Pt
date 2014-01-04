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

log_define("Pt.System.Timer")

namespace {

inline bool checkInterval(std::size_t interval, const Pt::Timespan& now)
{
    Pt::int64_t maxInterval = Pt::Timespan::maxMSecs();
    maxInterval -= now.toMSecs();
    return interval > static_cast<Pt::uint64_t>(maxInterval);
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
, _interval(0)
, _finished(0)
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
    return ! _finished.isNull();
}


std::size_t Timer::interval() const
{
    return _interval;
}


void Timer::start(std::size_t interval)
{
    if( isStarted() )
        this->stop();

    _interval = interval;
    log_debug("Timer started, interval: " << _interval);
    
    Timespan now = Clock::getSystemTicks();
    
    bool overrun = checkInterval(_interval, now);
    if(overrun)
    {
        Pt::int64_t maxTime = std::numeric_limits<Pt::int64_t>::max();
        _finished = Timespan(maxTime);
        log_debug("timer truncated to: " << _finished.toMSecs());
    }
    else
    {
        Timespan remaining( Pt::int64_t(_interval) * 1000 );
        _finished = now + remaining;
        log_debug("timer set to: " << _finished.toMSecs());
    }

    assert(_finished.toUSecs() > 0);
    
    if(_loop)
        _loop->onAttachTimer(*this);
}


void Timer::stop()
{
    _finished.setNull();

    if(_loop)
        _loop->onDetachTimer(*this);
}


bool Timer::update()
{
    if(isStarted() == false)
        return false;

    Timespan now = Clock::getSystemTicks();
    return this->update(now);
}


bool Timer::update(const Timespan& now)
{
    log_trace("Timer::update " << now.toUSecs() << " usecs");

    if(isStarted() == false)
        return false;

    bool hasElapsed = now >= _finished;
    log_debug("hasElapsed: " << hasElapsed);

    Timer::Sentry sentry(_sentry);

    while( isStarted() && now >= _finished )
    {
        log_debug("executing timer: " << _finished.toUSecs() << " usecs");

        bool overrun = checkInterval(_interval, now);
        if(overrun)
        {
            Pt::int64_t maxTime = std::numeric_limits<Pt::int64_t>::max();
            _finished = Timespan(maxTime);
            log_debug("timer truncated to: " << _finished.toMSecs());
        }
        else
        {
            Timespan remaining( Pt::int64_t(_interval) * 1000 );
            _finished += remaining;
            log_debug("timer set to: " << _finished.toMSecs());
        }

        assert(_finished.toUSecs() > 0);

        timeout().send();

        if( ! sentry )
        {
            log_debug("timer deleted, returning: " << hasElapsed);
            return hasElapsed;
        }
    }

    log_debug("Timer::update returns: " << hasElapsed);
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

} // namespace System

} // namespace Pt
