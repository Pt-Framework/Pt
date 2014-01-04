/*
 * Copyright (C) 2011 by Marc Boris Duerner
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
#include <algorithm>
#include "HttpBuffer.h"
#include <Pt/Http/HttpError.h>
#include <Pt/System/Logger.h>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <cassert>

log_define("Pt.Http.HttpBuffer")

namespace {

  std::string charToPrint(char ch)
  {
    std::string s;
    
    if (ch >= 32)
    {
      s += '<' ;
      s += ch; 
      s += '>';
    }

    s += '(';
    s += static_cast<unsigned>(static_cast<unsigned char>(ch));
    s += ')';
    return s;
  }

  void throwInvalidCharacter(char ch)
  {
    log_info("invalid character: " << charToPrint(ch));
    throw Pt::Http::HttpError("invalid HTTP message");
  }

}

namespace Pt {

namespace Http {

ChunkParser::ChunkParser()
: _state(&ChunkParser::onBegin)
, _chunkSize(0)
{
}


void ChunkParser::reset()
{
    _state = &ChunkParser::onBegin;
    _chunkSize = 0;
}


void ChunkParser::parse(char ch)
{
    if(_state)
        (this->*_state)(ch);
}


void ChunkParser::onBegin(char ch)
{
    log_trace("onBegin, ch=" << charToPrint(ch));

    if (ch >= '0' && ch <= '9')
    {
        _chunkSize = ch - '0';
        _state = &ChunkParser::onSize;
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        _chunkSize = ch - 'a' + 10;
        _state = &ChunkParser::onSize;
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        _chunkSize = ch - 'A' + 10;
        _state = &ChunkParser::onSize;
    }
    else
        throwInvalidCharacter(ch);
}

void ChunkParser::onSize(char ch)
{
    log_trace("onSize, ch=" << charToPrint(ch));

    if (ch >= '0' && ch <= '9')
    {
        _chunkSize = _chunkSize * 16 + (ch - '0');
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        _chunkSize = _chunkSize * 16 + (ch - 'a' + 10);
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        _chunkSize = _chunkSize * 16 + (ch - 'A' + 10);
    }
    else
    {
      log_debug("chunk size=" << _chunkSize);

      if (ch == '\r')
      {
          _state = &ChunkParser::onEndl;
      }
      else if (ch == '\n')
      {
          if (_chunkSize > 0)
              _state = &ChunkParser::onData;
          else
              _state = 0;
      }
      else
      {
          _state = &ChunkParser::onExtension;
      }
    }
}

void ChunkParser::onEndl(char ch)
{
    log_trace("onEndl, ch=" << charToPrint(ch));

    if (ch == '\n')
    {
      if (_chunkSize > 0)
          _state = &ChunkParser::onData;
      else
          _state = &ChunkParser::onTrailer;
    }
    else
        throwInvalidCharacter(ch);
}

void ChunkParser::onExtension(char ch)
{
    log_trace("onExtension");

    if (ch == '\r')
    {
        _state = &ChunkParser::onEndl;
    }
    else if (ch == '\n')
    {
      if (_chunkSize > 0)
          _state = &ChunkParser::onData;
      else
          _state = 0;
    }
}

void ChunkParser::onData(char ch)
{
    log_trace("onData, ch=" << charToPrint(ch));

    if (ch == '\r')
    {
        log_debug("=> onDataEnd");
        _state = &ChunkParser::onDataEnd;
    }
    else if (ch == '\n')
    {
        log_debug("=> onBegin");
        _state = &ChunkParser::onBegin;
    }
    else
        throwInvalidCharacter(ch);
}

void ChunkParser::onDataEnd(char ch)
{
    log_trace("onDataEnd, ch=" << charToPrint(ch));

    if (ch == '\n')
    {
        log_debug("=> onBegin");
        _state = &ChunkParser::onBegin;
    }
    else
        throwInvalidCharacter(ch);
}

void ChunkParser::onTrailer(char ch)
{
    log_trace("onTrailer, ch=" << charToPrint(ch));

    // @todo Report trailer fields by adding them to the other header fields.
    //       HttpBuffer and ChunkParser need a reference to a MessageHeader.

    if (ch == '\n')
        _state = 0;
    else if (ch == '\r')
        ;
    else
        _state = &ChunkParser::onTrailerData;
}

void ChunkParser::onTrailerData(char ch)
{
    log_trace("onTrailerData, ch=" << charToPrint(ch));

    // the trailer is actually ignored
    if (ch == '\n')
        _state = &ChunkParser::onTrailer;
}




const unsigned int HttpBuffer::MaxPutback = 4;

void HttpBuffer::beginBody(const MessageHeader& reply)
{
    log_trace("HttpBuffer::beginBody()");
    _chunkParser.reset();

    setg(0,0,0);

    _keepAlive = reply.isKeepAlive();
    _contentLength = reply.contentLength();
    _chunked = reply.isChunked();

    log_debug("keep-alive: " << _keepAlive);
    log_debug("chunked: " << _chunked);
    log_debug("content-length: " << _contentLength);
}


bool HttpBuffer::isEnd() const
{
    log_trace("HttpBuffer::isEnd()");
    if(_chunked)
        return _chunkParser.end();

    return _contentLength == 0;
}


void HttpBuffer::import(std::streamsize n)
{
    log_trace("HttpBuffer::import(" << n << ")");

    if( ! _sbuf)
        return;

    if(n == 0)
    {
        n = _sbuf->in_avail();
        log_debug("available: " << n);
    }

    if(n < 0)
        n = 0;

    // Move unread bytes and putback to front
    std::size_t putback  = MaxPutback;
    std::size_t leftover = 0;
    
    if( this->gptr() ) 
    {
        putback = std::min<std::size_t>( this->gptr() - this->eback(), MaxPutback);
        char* to = _buffer + MaxPutback - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );

        this->setg( _buffer + (MaxPutback - putback),  // start of get area
                    _buffer + MaxPutback,              // gptr position
                    _buffer + MaxPutback + leftover ); // end of get area
    }

    if(_chunked && _contentLength == 0)
    {
        log_debug("getting next chunk");
        _contentLength = 0;
        while(n-- && ! _chunkParser.end())
        {
            char ch = _sbuf->sbumpc();
            _chunkParser.parse(ch);
            if( _chunkParser.hasChunk() )
            {
                _contentLength = _chunkParser.chunkSize();
                break;
            }
        }
    }

    std::streamsize unused = sizeof(_buffer) - (MaxPutback + leftover);
    log_debug("unused buffer area: " << unused);

    // read no more than unused space in buffer area
    if( n > unused )
        n = unused;

    log_debug("content-length: " << _contentLength);

    // read no more than content length
    if( static_cast<std::size_t>(n) > _contentLength )
        n = _contentLength;

    if( this->isEnd() )
    {
        log_trace("received all content -> EOF");
        return;
    }

    log_debug("http buffer refill: " << n);
    if(n == 0)
        return;

    n = _sbuf->sgetn(_buffer + MaxPutback + leftover, n);

    setg(_buffer + MaxPutback - putback, // eback - start of get area
         _buffer + MaxPutback,           // gptr - current position
         _buffer + MaxPutback + n);      // egptr - end of get area

    _contentLength -= static_cast<std::size_t>(n);
    log_debug("remaining content length: " << _contentLength);
}


HttpBuffer::int_type HttpBuffer::underflow()
{ 
    log_trace("HttpBuffer::underflow()");

    if(this->gptr() < this->egptr())
        return traits_type::to_int_type(*(this->gptr()));

    import( sizeof(_buffer) );

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    return traits_type::eof();
}


std::streamsize HttpBuffer::showmanyc()
{
    // TODO: return -1 if the underlying stream is EOF, otherwise return 
    // always 0, because we can't tell if its the body or chunked encoding
    // statements

    // return _sbuf && _sbuf->in_avail() >= 0 ? 0 : -1;

    return 0;
}

} // namespace Http

} // namespace Pt
