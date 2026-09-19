/* Copyright (C) 2007 by Tommi Maekitalo
   Copyright (C) 2007-2026 by Marc Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_BLOB_H
#define PT_BLOB_H

#include <Pt/SmartPtr.h>
#include <Pt/RefCounted.h>
#include <cstddef>
#include <cstring>
#include <cstdlib>

namespace Pt {

namespace Db {

/** @brief Shared implementation of blob data.
*/
class IBlob : public RefCounted
{
    public:
        /** @brief Destructor.
        */
        virtual ~IBlob() {}

        /** @brief Copies @a len bytes from @a data into this blob.
        */
        virtual void assign(const char* data, std::size_t len) = 0;

        /** @brief Creates a new implementation of the same type.
        */
        virtual IBlob* create() const = 0;

        /** @brief Destroys an instance created by %create().
        */
        virtual void destroy() = 0;

        /** @brief Returns the size of the blob data.
        */
        std::size_t size() const
        { return _size; }

        /** @brief Returns a pointer to the blob data, or 0 if empty.
        */
        const char* data() const
        { return _data; }

        /** @brief Returns true if both instances contain the same data.
        */
        bool operator==(const IBlob& other) const
        {
            return _size == other._size &&
                ( std::strncmp(_data, other._data, _size) == 0 );
        }

    protected:
        IBlob()
        : _data(0)
        , _size(0)
        { }

        char* _data;
        std::size_t _size;
};

/** @brief Default blob implementation using new and delete.
*/
class BlobImpl : public IBlob
{
    public:
        BlobImpl()
        { }

        ~BlobImpl()
        {
            delete[] _data;
        }

        virtual void assign(const char* data, std::size_t len)
        {
            if (len == 0)
            {
                delete[] _data;
                _data = 0;
                _size = 0;
                return;
            }

            if( len > this->size() )
            {
                delete[] _data;
                _data = new char[len];
            }

            std::memcpy(_data, data, len);
            _size = len;
        }

        virtual IBlob* create() const
        { return new BlobImpl(); }

        virtual void destroy()
        { delete this; }

        static BlobImpl* emptyInstance()
        {
            static BlobImpl empty(1);
            return &empty;
        }

    protected:
        BlobImpl(std::size_t n)
        { RefCounted::addRef(); }
};


/** @internal Initialize statics in BlobImpl during static initialization.
*/
static struct BlobStaticInitializer
{
    BlobStaticInitializer()
    {
        BlobImpl::emptyInstance();
    }
} pt_blob_static_initializer;


/** @brief Copy-on-write binary large object.

    %Blob holds binary column data as a shared, copy-on-write value.
    The default constructor is empty. The data constructor copies
    @a len bytes from @a data. The %IBlob constructor takes ownership
    of a custom implementation.

    %assign() replaces the bytes and copies on write when the value is
    shared. %data() returns the bytes or 0 when empty. %size() is the
    length. Equality compares the bytes.

    @ingroup Pt-Db-Results
*/
class Blob
{
    template <typename T>
    struct Release
    {
        void destroy(T* blob)
        { blob->destroy(); }
    };

    SmartPtr<IBlob, InternalRefCounted<IBlob>, Release<IBlob> > m_data;

public:
    /** @brief Creates an empty blob.
    */
    Blob()
    : m_data( BlobImpl::emptyInstance() )
    { }

    /** @brief Copies @a len bytes from @a data.
    */
    Blob(const char* data, std::size_t len)
    : m_data( new BlobImpl() )
    {
        m_data->assign(data, len);
    }

    /** @brief Takes ownership of the implementation @a b.
    */
    explicit Blob(IBlob* b)
    : m_data(b)
    { }

    /** @brief Replaces the bytes with @a len bytes from @a data.
    */
    void assign(const char* data, std::size_t len)
    {
        // copy-on-write
        if ( m_data->refs() > 1 )
        {
            m_data.reset( m_data->create() );
        }

        m_data->assign(data, len);
    }

    /** @brief Returns true if both blobs contain the same data.
    */
    bool operator==(const Blob& b) const
    {
        return *m_data == *b.m_data;
    }

    /** @brief Returns true if the blobs do not contain the same data.
    */
    bool operator!=(const Blob& b) const
    {
        return !operator==(b);
    }

    /** @brief Returns a pointer to the data, or 0 if empty.
    */
    const char* data() const
    {
        return  m_data->data();
    }

    /** @brief Returns the size of the data.
    */
    std::size_t size() const
    {
        return m_data->size();
    }
};

} // namespace Db

} // namespace Pt

#endif //PT_BLOB_H
