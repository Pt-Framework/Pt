/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#include "ClientImpl.h"
#include "Parser.h"

#include <Pt/Http/Client.h>
#include <Pt/Http/HttpError.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>

#include <cassert>

log_define("Pt.Http.Client")

namespace Pt {

namespace Http {

ClientImpl::ClientImpl()
: _hstate(Idle)
, _conn()
, _req(_conn)
, _reply(_conn)
, _requestCount(0)
{
}


void ClientImpl::setSecure(Ssl::Context& ctx)
{
    _conn.setSecure(ctx);
}


void ClientImpl::send(bool finished)
{
    if(_hstate == Idle || _hstate == OnRequestComplete)
    {
        // NOTE: might want to handle connect here in the future
        _hstate = OnRequest;
    }

    if(_hstate == OnRequest)
    {
        log_debug("begin sending request");
        _req.send(finished);

        log_debug("sent http request completed");
        _req.discard();
            
        if( _req.isFinished() )
        {
            ++_requestCount;
            _hstate = OnRequestComplete;
        }
        return;
    }

    log_error("sending HTTP request failed: " << _hstate);
    throw HttpError("HTTP message pending");
}


std::istream& ClientImpl::receive()
{
    if(_hstate == Idle)
    {
        // NOTE: might want to handle connect here in the future
        _hstate = OnRequest;
    }
 
    if(_hstate == OnRequest)
    {
        log_debug("begin sending cached request");
        _hstate = OnRequestEnd;
    }

    if(_hstate == OnRequestEnd)
    {
        log_debug("flushing cached request");
        _req.send(true);

        log_debug("request completed");
        _hstate = OnReply;
        ++_requestCount;
        _req.discard();
    }

    if(_hstate == OnReply || _hstate == OnRequestComplete || _hstate == OnReplyComplete)
    {
        _hstate = OnReply;

        _reply.clear();
        _reply.receive();

        log_debug("reply completed");
        assert(0 != _requestCount);

        if( 0 == --_requestCount)
            _hstate = Idle;
        else
            _hstate = OnReplyComplete;

        if( ! _conn.isConnected() )
        {
            log_debug("connection closed");
            // connection will reconnect automatically
        }
        
        return _reply.body();
    }

    throw HttpError("HTTP message pending");
    return _reply.body();
}


void ClientImpl::beginSend(bool finished)
{
    log_trace("beginSend: " << _hstate);
    
    if(_hstate == Idle || _hstate == OnRequestComplete)
    {
        // NOTE: might want to handle connect here in the future
        _hstate = OnRequest;
    }

    if(_hstate == OnRequest)
    {
        log_debug("begin sending http chunk");
        _req.beginSend(finished);
        return;
    }

    log_error("sending HTTP request failed: " << _hstate);
    throw HttpError("HTTP message pending");
}


MessageProgress ClientImpl::endSend()
{
    log_trace("endSend: " << _hstate);

    if(_hstate == OnRequest)
    {
        log_debug("sent http request");
        MessageProgress progress = _req.endSend();
        if( progress.finished() )
        {
            log_debug("sent http request completed");
            _req.discard();
            
            if( _req.isFinished() )
            {
                ++_requestCount;
                _hstate = OnRequestComplete;
                _reply.clear(); // TODO: find better place to clear reply !!!
            }
        }
            
        return progress;
    }

    throw HttpError("HTTP message pending");
    return MessageProgress();
}


void ClientImpl::beginReceive()
{
    log_debug("beginReceive: " << _hstate);
    
    if(_hstate == Idle)
    {
        // NOTE: might want to handle connect here in the future
        _hstate = OnRequest;
    }
 
    if(_hstate == OnRequest)
    {
        log_debug("begin sending cached request");

        ///_req.finish();
        _hstate = OnRequestEnd;
    }

    if(_hstate == OnRequestEnd)
    {
        log_debug("flushing cached request");
        _req.beginSend(true);
        return;
    }

    if(_hstate == OnReply || _hstate == OnRequestComplete || _hstate == OnReplyComplete)
    {
        _hstate = OnReply;
        _reply.beginReceive();
        return;
    }

    throw HttpError("HTTP message pending");
}


MessageProgress ClientImpl::endReceive()
{
    log_debug("endReceive: " << _hstate);

    if(_hstate == OnRequestEnd)
    {
        log_debug("flushed cached request");
        MessageProgress progress = _req.endSend();
        if( progress.finished() )
        {
            log_debug("request completed");
            _req.discard();

            _hstate = OnReply;
            ++_requestCount;
            _reply.clear(); // TODO: find better place to clear reply !!!
        }
        
        return MessageProgress();
    }

    if(_hstate == OnReply)
    {   
        log_debug("receiving header");
        MessageProgress progress = _reply.endReceive();

        if( progress.finished() )
        {
            log_debug("reply completed");
            assert(0 != _requestCount);

            if( 0 == --_requestCount)
                _hstate = Idle;
            else
                _hstate = OnReplyComplete;

            if( ! _conn.isConnected() )
            {
                log_debug("connection closed");
                // connection will reconnect automatically
            }
        }

        return progress;
    }

    throw HttpError("HTTP message pending");
    return MessageProgress();
}


void ClientImpl::cancel()
{
    _conn.cancel();
    _hstate = Idle;
    _requestCount = 0;
}

} // namespace Http

} // namespace Pt
