/*
 * Copyright (C) 2009 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef PT_HTTP_CHUNKEDREADER_H
#define PT_HTTP_CHUNKEDREADER_H

#include <streambuf>
#include <iostream>

namespace Pt
{
    namespace Http
    {
        class ChunkedReader : public std::streambuf
        {
                std::streambuf* _ib;
                char* _buffer;
                unsigned _bufsize;
                unsigned _chunkSize;

                void (ChunkedReader::*_state)();

                void onBegin();
                void onSize();
                void onEndl();
                void onExtension();
                void onData();
                void onDataEnd0();
                void onDataEnd();
                void onTrailer();
                void onTrailerData();

            public:
                explicit ChunkedReader(std::streambuf* ib, unsigned bufsize = 8192);

                void reset()      { _state = &ChunkedReader::onBegin; setg(0, 0, 0); }
                bool eod() const  { return _state == 0; }

                std::streamsize showmanyc();
                virtual int sync();
                virtual int_type overflow(int_type ch);
                virtual int_type underflow();

        };

        class ChunkedIStream : public std::istream
        {
                ChunkedReader _streambuf;

            public:
                explicit ChunkedIStream(std::streambuf* ib)
                  : std::istream(&_streambuf),
                    _streambuf(ib)
                  { }

                void reset()        { _streambuf.reset(); clear(); }
                bool eod() const    { return _streambuf.eod(); }
        };

    }
}

#endif // PT_HTTP_CHUNKEDREADER_H
