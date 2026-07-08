/* Copyright (C) 2008 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "TarHeader.h"
#include <Pt/System/TarReader.h>

#include <Pt/IOError.h>

#include <algorithm>
#include <cstring>
#include <string>

namespace Pt {

namespace System {

///////////////////////////////////////////////////////////////////////
// Pax extended header parsing
///////////////////////////////////////////////////////////////////////

namespace {

struct PaxOverrides
{
    std::string path;
    std::string linkPath;
    std::size_t size;
    double      mtime;
    bool        hasPath;
    bool        hasLinkPath;
    bool        hasSize;
    bool        hasMtime;

    PaxOverrides()
    : size(0)
    , mtime(0.0)
    , hasPath(false)
    , hasLinkPath(false)
    , hasSize(false)
    , hasMtime(false)
    { }

    void clear()
    {
        path.clear();
        linkPath.clear();
        size        = 0;
        mtime       = 0.0;
        hasPath     = false;
        hasLinkPath = false;
        hasSize     = false;
        hasMtime    = false;
    }
};


static void parsePaxData(const std::string& data, PaxOverrides& pax)
{
    // Each record: "<decimal_len> <key>=<value>\n"
    // decimal_len includes itself, the space, the key=value and the newline.
    std::size_t pos = 0;
    while(pos < data.size())
    {
        std::size_t spacePos = data.find(' ', pos);
        if(spacePos == std::string::npos || spacePos == pos)
            break;

        std::size_t recLen = 0;
        for(std::size_t i = pos; i < spacePos; ++i)
        {
            if(data[i] < '0' || data[i] > '9')
                break;
            recLen = recLen * 10u + static_cast<std::size_t>(data[i] - '0');
        }

        if(recLen == 0 || pos + recLen > data.size())
            break;

        std::size_t eqPos = data.find('=', spacePos + 1);
        if(eqPos == std::string::npos || eqPos >= pos + recLen)
        {
            pos += recLen;
            continue;
        }

        std::string key   = data.substr(spacePos + 1, eqPos - (spacePos + 1));
        std::size_t valEnd = pos + recLen - 1; // exclude trailing '\n'
        std::string value = data.substr(eqPos + 1, valEnd - (eqPos + 1));

        if(key == "path")
        {
            pax.path    = value;
            pax.hasPath = true;
        }
        else if(key == "linkpath")
        {
            pax.linkPath    = value;
            pax.hasLinkPath = true;
        }
        else if(key == "size")
        {
            pax.size    = static_cast<std::size_t>(std::stoul(value));
            pax.hasSize = true;
        }
        else if(key == "mtime")
        {
            pax.mtime    = std::stod(value);
            pax.hasMtime = true;
        }

        pos += recLen;
    }
}

} // namespace

///////////////////////////////////////////////////////////////////////
// TarReaderImpl
///////////////////////////////////////////////////////////////////////

class TarReaderImpl
{
  public:
    TarReaderImpl()
    : _source(0)
    , _state(OnBegin)
    , _nullBlocks(0)
    , _skipRemaining(0)
    , _padding(0)
    , _bufData(0)
    , _bufEnd(0)
    { }

    explicit TarReaderImpl(std::istream& is)
    : _source(&is)
    , _state(OnHeader)
    , _nullBlocks(0)
    , _skipRemaining(0)
    , _padding(0)
    , _bufData(0)
    , _bufEnd(0)
    { }

    void attach(std::istream& is)
    {
        _source = &is;
        _state  = OnHeader;
    }

    void detach()
    {
        _source = 0;
    }

    void reset()
    {
        _source        = 0;
        _state         = OnBegin;
        _nullBlocks    = 0;
        _skipRemaining = 0;
        _padding       = 0;
        _bufData       = 0;
        _bufEnd        = 0;
        _paxBuf.clear();
        _pax.clear();
        _entry.clear();
    }

    void reset(std::istream& is)
    {
        reset();
        _source = &is;
        _state  = OnHeader;
    }

    bool isEnd() const
    {
        return _state == OnEnd;
    }

    const TarEntry* advance(std::streamsize importSize = 0)
    {
        if(_state == OnEnd || ! _source)
            return 0;

        for(;;)
        {
            switch(_state)
            {
                case OnBegin:
                {
                    _state = OnHeader;
                    break;
                }

                case OnHeader:
                {
                    if( ! importHeader(512, importSize) )
                        return 0;

                    const char* block = _buf + _bufData;

                    if( tarIsNullBlock(block) )
                    {
                        ++_nullBlocks;
                        consume(512);
                        if(_nullBlocks >= 2)
                        {
                            _state = OnEnd;
                            return 0;
                        }
                        break; // try next block
                    }

                    _nullBlocks = 0;

                    if( ! tarVerifyChecksum(block) )
                        throw Pt::IOError("tar: invalid header checksum");

                    bool isHeader = parseHeader(block);
                    consume(512);

                    if(isHeader)
                    {
                        if(_state == OnData)
                            setData();

                        return &_entry;
                    }

                    break; // continue for pax / skip states
                }

                case OnPaxData:
                {
                    std::size_t n = std::min(available(), _skipRemaining);
                    if(n > 0)
                    {
                        _paxBuf.append(_buf + _bufData, n);
                        consume(n);
                        _skipRemaining -= n;
                    }

                    if(_skipRemaining == 0)
                    {
                        parsePaxData(_paxBuf, _pax);
                        _state = OnPaxPadding;
                        break;
                    }

                    if( ! import(importSize) )
                        return 0;

                    break;
                }

                case OnPaxPadding:
                {
                    if(_padding == 0)
                    {
                        _state = OnHeader;
                        break;
                    }

                    std::size_t n = std::min(available(), _padding);
                    consume(n);
                    _padding -= n;

                    if(_padding == 0)
                    {
                        _state = OnHeader;
                        break;
                    }

                    if( ! import(importSize) )
                        return 0;

                    break;
                }

                case OnSkip:
                {
                    std::size_t n = std::min(available(), _skipRemaining);
                    consume(n);
                    _skipRemaining -= n;

                    if(_skipRemaining == 0)
                    {
                        _state = OnHeader;
                        break;
                    }

                    if( ! import(importSize) )
                        return 0;

                    break;
                }

                case OnData:
                {
                    if(_entry.avail() > 0)
                    {
                        _bufData += _entry.avail();
                        _entry.setRemaining(_entry.remaining() - _entry.avail());
                        _entry.setData(0, 0);
                    }

                    if(_entry.remaining() == 0)
                    {
                        _state = OnPadding;
                        break;
                    }

                    if( import(importSize) )
                        setData();

                    return &_entry;
                }

                case OnPadding:
                {
                    if(_padding == 0)
                    {
                        _state = OnHeader;
                        break;
                    }

                    std::size_t n = std::min(available(), _padding);
                    consume(n);
                    _padding -= n;

                    if(_padding == 0)
                    {
                        _state = OnHeader;
                        break;
                    }

                    if( ! import(importSize) )
                        return 0;

                    break;
                }

                case OnEnd:
                    return 0;
            }
        }
    }

  private:
    std::size_t available() const
    {
        return _bufEnd - _bufData;
    }

    void consume(std::size_t n)
    {
        _bufData += n;
    }

    std::streamsize import(std::streamsize& importSize)
    {
        if( ! _source || ! _source->rdbuf() )
            return 0;

        std::streamsize avail = (importSize > 0) ? importSize
                                                 : _source->rdbuf()->in_avail();
        if(avail <= 0)
            return 0;

        if(_bufData > 0)
        {
            std::size_t len = _bufEnd - _bufData;
            if(len > 0)
                std::memmove(_buf, _buf + _bufData, len);

            _bufEnd  = len;
            _bufData = 0;
        }

        std::size_t space = BufSize - _bufEnd;
        if(space == 0)
            return 0;

        std::streamsize toRead = std::min(static_cast<std::streamsize>(space), avail);
        std::streamsize n = _source->rdbuf()->sgetn(_buf + _bufEnd, toRead);
        if(n > 0)
        {
            _bufEnd += static_cast<std::size_t>(n);
            if(importSize > 0)
                importSize -= n;
        }
        return n;
    }

    bool importHeader(std::size_t needed, std::streamsize& importSize)
    {
        while(available() < needed)
        {
            if( ! import(importSize) )
                return available() >= needed;
        }
        return true;
    }

    void setData()
    {
        std::size_t a = std::min( _entry.remaining(), available() );
        _entry.setData(a > 0 ? _buf + _bufData : 0, a);
    }

    // Returns true when a TarEntry is ready to be returned, false when
    // processing should continue (pax / skip / global header).
    bool parseHeader(const char* block)
    {
        const auto* h = reinterpret_cast<const UStarHeader*>(block);

        // --- path ---
        std::string path;
        if(_pax.hasPath)
        {
            path = _pax.path;
        }
        else
        {
            std::string name(h->name, tarFieldLen(h->name, sizeof(h->name)));
            std::string prefix(h->prefix,
                               tarFieldLen(h->prefix, sizeof(h->prefix)));
            path = prefix.empty() ? name : prefix + "/" + name;
        }

        // --- size ---
        std::size_t size = _pax.hasSize
            ? _pax.size
            : tarParseOctal(h->size, sizeof(h->size));

        // --- mode ---
        std::size_t mode = tarParseOctal(h->mode, sizeof(h->mode));

        // --- mtime ---
        Pt::DateTime mtime;
        if(_pax.hasMtime)
            mtime = tarEpochToDatetime(static_cast<time_t>(_pax.mtime));
        else
            mtime = tarEpochToDatetime(
                        static_cast<time_t>(
                            tarParseOctal(h->mtime, sizeof(h->mtime))));

        // --- link target ---
        std::string linkTarget;
        if(_pax.hasLinkPath)
            linkTarget = _pax.linkPath;
        else
            linkTarget = std::string(h->linkname,
                                     tarFieldLen(h->linkname,
                                                 sizeof(h->linkname)));

        _pax.clear();

        char typeflag = h->typeflag;
        if(typeflag == '\0')
            typeflag = '0';

        auto perms =
            static_cast<Pt::System::FileInfo::Perms>(mode & 0777u);

        switch(typeflag)
        {
            case '0': // regular file
            {
                _entry.setPath( Pt::System::Path(path.c_str()) );
                _entry.setSize(size);
                _entry.setRemaining(size);
                _entry.setData(0, 0);
                _entry.setType(TarEntry::File);
                _entry.setLinkTarget(Pt::System::Path());
                _entry.setPermissions(perms);
                _entry.setMtime(mtime);
                _padding = (512u - (size % 512u)) % 512u;
                _state   = OnData;
                return true;
            }

            case '2': // symbolic link
            {
                _entry.setPath( Pt::System::Path(path.c_str()) );
                _entry.setSize(0);
                _entry.setRemaining(0);
                _entry.setData(0, 0);
                _entry.setType(TarEntry::Link);
                _entry.setLinkTarget( Pt::System::Path(linkTarget.c_str()) );
                _entry.setPermissions(perms);
                _entry.setMtime(mtime);
                _state = OnHeader;
                return true;
            }

            case '5': // directory
            {
                while( ! path.empty() && path.back() == '/')
                    path.pop_back();
                _entry.setPath( Pt::System::Path(path.c_str()) );
                _entry.setSize(0);
                _entry.setRemaining(0);
                _entry.setData(0, 0);
                _entry.setType(TarEntry::Directory);
                _entry.setLinkTarget(Pt::System::Path());
                _entry.setPermissions(perms);
                _entry.setMtime(mtime);
                _state = OnHeader;
                return true;
            }

            case 'x': // Pax extended header (local, applies to next entry)
            {
                _paxBuf.clear();
                _skipRemaining = size;
                _padding       = (512u - (size % 512u)) % 512u;
                _state         = OnPaxData;
                return false;
            }

            case 'g': // Pax global header (skip)
            {
                std::size_t pad = (512u - (size % 512u)) % 512u;
                _skipRemaining  = size + pad;
                _state          = OnSkip;
                return false;
            }

            case '1': // hard link
            {
                _entry.setPath( Pt::System::Path(path.c_str()) );
                _entry.setSize(0);
                _entry.setRemaining(0);
                _entry.setData(0, 0);
                _entry.setType(TarEntry::Hardlink);
                _entry.setLinkTarget( Pt::System::Path(linkTarget.c_str()) );
                _entry.setMtime(mtime);
                _state = OnHeader;
                return true;
            }

            default: // unknown type: skip data blocks
            {
                std::size_t pad = (512u - (size % 512u)) % 512u;
                _skipRemaining  = size + pad;
                _state          = _skipRemaining > 0 ? OnSkip : OnHeader;
                return false;
            }
        }
    }

  private:
    enum State
    {
        OnBegin,
        OnHeader,
        OnPaxData,
        OnPaxPadding,
        OnSkip,
        OnData,
        OnPadding,
        OnEnd
    };

    static const std::size_t BufSize = 8192;

    std::istream* _source;
    State         _state;
    int           _nullBlocks;
    std::size_t   _skipRemaining;
    std::size_t   _padding;

    char          _buf[BufSize];
    std::size_t   _bufData;
    std::size_t   _bufEnd;

    std::string    _paxBuf;
    PaxOverrides   _pax;
    TarEntry       _entry;
};

///////////////////////////////////////////////////////////////////////
// TarReader
///////////////////////////////////////////////////////////////////////

TarReader::TarReader()
: _impl(new TarReaderImpl())
{ }


TarReader::TarReader(std::istream& is)
: _impl(new TarReaderImpl(is))
{ }


TarReader::~TarReader()
{
    delete _impl;
}


void TarReader::attach(std::istream& is)
{
    _impl->attach(is);
}


void TarReader::detach()
{
    _impl->detach();
}


void TarReader::reset()
{
    _impl->reset();
}


void TarReader::reset(std::istream& is)
{
    _impl->reset(is);
}


const TarEntry* TarReader::advance(std::streamsize importSize)
{
    return _impl->advance(importSize);
}


bool TarReader::isEnd() const
{
    return _impl->isEnd();
}

} // namespace System

} // namespace Pt
