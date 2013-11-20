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

#ifndef Pt_Http_HttpBuffer_h
#define Pt_Http_HttpBuffer_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Message.h>
#include <streambuf>

namespace Pt {

namespace Http {

/** @internal @brief Parses HTTP chunked encoding format.

    @todo Pass a reference to the MessageHeader, so the trailer fields
          can be added, which follow the HTTP body
*/
class ChunkParser
{
    public: 
        ChunkParser();

        void reset();

        void parse(char ch);

        bool hasChunk() const
        { return _state == &ChunkParser::onData; }

        std::size_t chunkSize() const
        { return _chunkSize; }

        bool end() const  
        { return _state == 0; }

    private:
        void onBegin(char ch);
        void onSize(char ch);
        void onEndl(char ch);
        void onExtension(char ch);
        void onData(char ch);
        void onDataEnd(char ch);
        void onTrailer(char ch);
        void onTrailerData(char ch);

    private:
        void (ChunkParser::*_state)(char ch);
        std::size_t _chunkSize;
};

class HttpBuffer : public std::streambuf
{
    static const unsigned int MaxPutback;

    public:
        HttpBuffer()
        : _sbuf(0)
        , _contentLength(0)
        , _chunked(false)
        , _keepAlive(false)
        {
            setg(0,0,0);
        }

        ~HttpBuffer()
        { }
        
        void attach(std::streambuf& sbuf)
        { _sbuf = &sbuf; }

        std::streambuf* buffer()
        { return _sbuf; }

        // TODO: same reset semantics as XmlReader
        void reset()
        { 
            _contentLength = 0;
            _chunked = false;
            _keepAlive = false;
        }

        void beginBody(const MessageHeader& reply);

        void import(std::streamsize n = 0);

        bool isEnd() const;

    protected:
        virtual int_type underflow();

        virtual std::streamsize showmanyc();

    private:
        ChunkParser _chunkParser;
        std::streambuf* _sbuf;
        char _buffer[4096];
        std::size_t _contentLength;
        bool _chunked;
        bool _keepAlive;
};

} // namespace Http

} // namespace Pt

#endif
