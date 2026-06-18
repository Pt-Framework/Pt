/*
  Copyright (C) 2006 by Tommi Maekitalo
  Copyright (C) 2006-2026 by Marc Boris Duerner

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

#include <Pt/Db/Cursor.h>
#include <Pt/Db/ICursor.h>
#include <Pt/Db/IConnection.h>

namespace Pt {

namespace Db {

Cursor::Cursor(ICursor* cursor, size_type batchSize)
: _cursor(cursor)
, _batchSize(batchSize)
{}


bool Cursor::fetch()
{
    _batch = _cursor->connection()->fetchBatch(*_cursor, _batchSize);
    return ! _batch.empty();
}


Signal<>& Cursor::fetchFinished()
{
    return _cursor->fetchFinished();
}


void Cursor::beginFetch()
{
    _cursor->connection()->beginBatchFetch(*_cursor, _batchSize);
}


Result& Cursor::endFetch()
{
    _batch = _cursor->connection()->endBatchFetch(*_cursor);
    if(_batch.empty())
        _cursor->connection()->closeCursor(*_cursor);
    return _batch;
}


void Cursor::close()
{
    if(_cursor && _cursor->isOpen())
        _cursor->connection()->closeCursor(*_cursor);
}


Cursor::Iterator Cursor::begin()
{
    return CursorIterator(*this);
}


Cursor::Iterator Cursor::end()
{
    return CursorIterator();
}


CursorIterator::CursorIterator(const Cursor& cursor)
: _cursor(cursor)
, _index(0)
{
    if( ! _cursor.fetch())
        _cursor = Cursor();
    else
        _current = _cursor.result()[0];
}


bool CursorIterator::operator==(const CursorIterator& other) const
{
    return _cursor.impl() == other._cursor.impl();
}


CursorIterator& CursorIterator::operator++()
{
    ++_index;
    if(_index >= _cursor.result().size())
    {
        if( ! _cursor.fetch())
        {
            _cursor = Cursor();
            _index = 0;
            return *this;
        }
        _index = 0;
    }
    _current = _cursor.result()[_index];
    return *this;
}

} // namespace Db

} // namespace Pt
