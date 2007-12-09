/***************************************************************************
 *   Copyright (C) 2007 by Tommi Mäkitalo                                  *
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

#include <string>
#include <Pt/SmartPtr.h>

namespace Pt {

/*! A class to hold binary data.
    The data is represented by a plain pointer to the data
    plus the length of the data. The class can be used for holding information about
    tiles coming from a database or any other tile-source. A blob is the owner of the data
    which is referenced by itself. Therefore the blob is responsible for destroying the referenced
    data once it is destroyed itself.
    The Blob contains a reference counter, which is incremented by the copy constructor and
    the assignement operator and decremented on destruction. This avoid copying of the data
    the Blob holds and makes assignement a cheap operation.
*/
class Blob
{
    SmartPtr<std::string, ExternalRefCounted<std::string> > m_dataptr;

public:
    /*!
        Default constructor which creates an empty blob object.
    */
    Blob()
    { }

    /*! 
        @param data The pointer to the data from which to create the Blob.
        @param len The length of the data.
    */
    Blob(const char* data, unsigned len)
        : m_dataptr(new std::string(data, len))
    { }

    /* Assign raw data to a blob.
       @param data A pointer to the data of which this blob should create a copy.
       @param len The length of the data of which this data should create a copy. */
    void assign(const char* data, size_t len) {
        if (!m_dataptr || m_dataptr.refs() > 1)
            m_dataptr = new std::string(data, len);
        else
            m_dataptr->assign(data, len);
    }
    /*! Comparison-operator.
        @param b The Blob to which to compare the current Blob.
        @return true, if the Blobs point to the same data (pointer comparison)
    */
    bool operator==(const Blob& b) const {
        return m_dataptr.getPointer() == b.m_dataptr.getPointer()
            || m_dataptr.getPointer()
             && m_dataptr.getPointer()
             && *m_dataptr == *m_dataptr;
    }

    bool operator!=(const Blob& b) const {
        return !operator==(b);
    }

    /*!
        Returns a pointer to the data or 0 if no data is set.
     */
    const char* data() const
    {
      return m_dataptr.getPointer() ? m_dataptr->data() : 0;
    }

    /*!
        Returns the size of the data
     */
    unsigned size() const
    {
      return m_dataptr.getPointer() ? m_dataptr->size() : 0;
    }

};

} // namespace Pt

#endif //PT_BLOB_H

