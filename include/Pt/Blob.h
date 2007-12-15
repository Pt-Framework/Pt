/***************************************************************************
 *   Copyright (C) 2007 by Tommi Maekitalo                                 *
 *   Copyright (C) 2007 by Marc Duerner                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_BLOB_H
#define PT_BLOB_H

#include <Pt/SmartPtr.h>
#include <Pt/RefCounted.h>
#include <cstring>
#include <cstdlib>

namespace Pt {

class IBlob : public RefCounted
{
    public:
        virtual ~IBlob() {}

        virtual void assign(const char* data, size_t len) = 0;

        size_t size() const
        { return _size; }

        const char* data() const
        { return _data; }

        bool operator==(const IBlob& other) const
        {
            return _size == other._size &&
                   ( std::strncmp(_data, other._data, _size) == 0 );
        }

    protected:
        IBlob()
        :_data(0)
        , _size(0)
        { }

        char* _data;
        size_t _size;
};


class BlobData : public IBlob
{
    public:
        BlobData()
        { }

        virtual void assign(const char* data, size_t len)
        {
            if( len > this->size() )
            {
                if(_data)
                    std::free(_data);

                _data = (char*)malloc(len);
            }

            std::memcpy(_data, data, len);
            _size = len;
        }

        static BlobData* emptyInstance()
        {
            static BlobData empty(1);
            return &empty;
        }

    protected:
        BlobData(size_t refs)
        { this->addRef(); }
};


/** @internal Initialize statics in BlobData.

    Thread-safety.
*/
static struct InitBlobData
{
    InitBlobData()
    { BlobData::emptyInstance(); }
} init;


/** @brief A class to hold binary data.

    The data is represented by a plain pointer to the data plus the length of
    the data. The class can be used for holding information about tiles coming
    from a database or any other tile-source. A blob is the owner of the data
    which is referenced by itself. Therefore the blob is responsible for
    destroying the referenced data once it is destroyed itself. The Blob
    contains a reference counter, which is incremented by the copy constructor
    and the assignement operator and decremented on destruction. This avoid
    copying of the data the Blob holds and makes assignement a cheap operation.
*/
class Blob
{
    SmartPtr< IBlob, InternalRefCounted<IBlob> > m_data;

public:
    Blob()
    {
        m_data = BlobData::emptyInstance();
    }

    Blob(const char* data, size_t len)
    {
        m_data = new BlobData();
        m_data->assign(data, len);
    }

    Blob(IBlob* b)
    {
        m_data = b;
    }

    void assign(const char* data, size_t len)
    {
        if ( m_data->refs() > 1 )
        {
            m_data = new BlobData();
        }

        m_data->assign(data, len);
    }

    bool operator==(const Blob& b) const
    {
        return *m_data == *m_data;
    }

    bool operator!=(const Blob& b) const
    {
        return !operator==(b);
    }

    /** Returns a pointer to the data or 0 if no data is set.
     */
    const char* data() const
    {
      return  m_data->data();
    }

    /** Returns the size of the data
     */
    size_t size() const
    {
      return m_data->size();
    }

};

} // namespace Pt

#endif //PT_BLOB_H

