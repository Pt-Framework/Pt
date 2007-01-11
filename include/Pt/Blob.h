#ifndef PT_BLOB_H
#define PT_BLOB_H

#include <stddef.h>
#include <memory.h>

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

public:
    /*!
        Default constructor which creates an empty blob object.
    */
    Blob() : m_data(0), m_length(0), m_size(0), m_refcnt(new int(1))
    { }

    /*! This is a constructor with ownership-transfer semantics.
        This means that the caller of this constructer gives up ownership of
        the referenced data. As a consequence, the caller must not destroy
        the referenced data - the Blob is now responsible for that.
        @param data The pointer to the data from which to create the Blob.
        @param lenght The length of the data.
    */
    Blob(const char* data, size_t len) : m_data(data), m_length(len), m_size(len), m_refcnt(new int(1))
    { }

    /*! This is the copy-constructor which creates a deep-copy of a Blob.
        There is no data copied, only the reference counter gets incremented.
    */
    Blob(const Blob& aBlob) : m_data(aBlob.m_data), m_length(aBlob.m_length), m_size(aBlob.m_size),
                  m_refcnt(aBlob.m_refcnt)
    {
        (*m_refcnt)++;
    }

    /*! Destroys this Blob.
        As the Blob is defined to be the owner of the
        referenced data, it is responsible for destroying it.
        The data is only destroyed, if the reference counter drop to 0.
    */
    virtual ~Blob() {
        dispose();
    };

    /*! Assignment-operator which just increments the refcounter of the Blob,
        so, no copying of data is performed.
    */
    Blob& operator=(const Blob& b) {
        // self assignement is handled implicit by first incrementing the ref counter!
        // (in 99.9% you don't do self assignement so a explicit
        // check would be a waste of time )
        (*b.m_refcnt)++;
        dispose();
        m_data = b.m_data;
        m_length = b.m_length;
        m_size = b.m_size;
        m_refcnt = b.m_refcnt;
        return *this;
    }

    void assign(const char* data, size_t len) {        
        if (*m_refcnt == 1 && len <= m_size) {
            m_length = len;
            memcpy((void*) m_data, data, len);
            return;
        }
        dispose();
        m_data = new char[len];
        m_length = len;
        m_size = len;
        memcpy((void*) m_data, data, len);
        m_refcnt = new int(1);
    }
    /*! Comparison-operator.
        The Blobs are assumed to be equal, if they point to the
        same data (pointer comparison). Note that the assignment operator and the copy-
        constructor make a deep-copy of the data. As a result this comparison operator
        doesn't regard blobs created this way as being equal.
        @param Blob The Blob to which to compare the current Blob.
        @return true, if the Blobs point to the same data (pointer comparison)
    */
    bool operator==(const Blob& b) const {
        return b.m_data == this->m_data && b.m_length == this->m_length;
    }

    bool operator!=(const Blob& b) const {
        return !operator==(b);
    }

    /*!
        The pointer to the content of this Blob.
    */
    const char* m_data;

    /*!
        The length of the content of this Blob.
    */
    size_t m_length;
    size_t m_size;

    /*!
       The reference counter for the Blob.
    */
    int *m_refcnt;

private:
    /*! Deletion of the Blob.
        The reference counter gets decremented, if it drops to 0 then the data of the Blob
        is destroyed otherwise nothing more happens
    */
    void dispose()
    {
        --(*m_refcnt);
        if (*m_refcnt == 0)
        {
            delete[] m_data;
            delete m_refcnt;
        }
    }

};

} // namespace Pt

#endif //PTV_BLOB_H

