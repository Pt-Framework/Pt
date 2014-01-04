/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#ifndef PT_SMARTPTR_H
#define PT_SMARTPTR_H

#include <Pt/Api.h>
#include <Pt/Atomicity.h>
#include <Pt/SerializationInfo.h>

namespace Pt {

/**
    \param T The managed object type
*/
template <typename T>
/** \brief Reference linking.

    Reference linking means that no counter is required to keep track of
    the smart pointer objects, but all smart pointers form a linked
    list. When the list becomes empty the raw pointer si deleted. This
    Model has the advantage that it does not need to allocate memory,
    but is prone to circular dependencies.
*/
class RefLinked
{
    private:
        mutable const RefLinked* prev;
        mutable const RefLinked* next;

    protected:
        RefLinked()
        : prev(0),
          next(0)
          { }

        //! \brief Unlink a smart pointer from a managed object
        bool unlink(T* object)
        {
            bool ret = false;
            if (object)
            {
                if (next == this)
                {
                    ret = true;
                }
                else
                {
                    next->prev = prev;
                    prev->next = next;
                }
                next = prev = this;
            }
            return ret;
        }

        //! \brief Link a smart pointer to a managed object
        void link(const RefLinked& ptr, T* object)
        {
            if (object)
            {
                prev = &ptr;
                next = ptr.next;
                prev->next = this;
                next->prev = this;
            }
        }
};


/**
    \param T The managed object type
*/
template <typename T>
/** \brief Intrusive reference counting.

    Intrusive reference couting means that the reference count is part of the
    managed heap object. Linking and unlinking will only increase and decrease this
    counter, but not delete it. The managed object needs to implement the functions
    release() and addRef() and must delete itself if the counter reaches zero.
*/
class InternalRefCounted
{
    protected:
        //! \brief unlink a smart pointer from a managed object
        bool unlink(T* object)
        {
            if (object)
                object->release();
            return false;
        }

        //! \brief link a smart pointer to a managed object
        void link(const InternalRefCounted& ptr, T* object)
        {
            if (object)
                object->addRef();
        }
};


/**
    \param T The managed object type
*/
template <typename T>
/** \brief Non-intrusive reference counting.

    Non-intrusive reference couting means that the reference count is not part of the
    managed heap object but part of the policy. Linking and unlinking will increase and
    decrease the policies counter and delete the managed object if it reaches zero. A
    small amount of memory needs to be allocated for the counter variable.
*/
class ExternalRefCounted
{
    public:
        int refs() const
        { return _count ? *_count : 0; }

    protected:
        ExternalRefCounted()
        : _count(0)
        { }

        //! \brief unlink a smart pointer from a managed object
        bool unlink(T* object)
        {
            if ( _count && --*_count <= 0)
            {
                delete _count;
                // no need to set _count to 0 since the pointer is either
                // destroyed or another object is linked in
                return true;
            }
            else
                return false;
        }

        //! \brief link a smart pointer to a managed object
        void link(const ExternalRefCounted& ptr, T* object)
        {
            if(object)
            {
                if(ptr._count == 0) {
                    _count = new unsigned(1);
                }
                else
                {
                    _count = ptr._count;
                    ++*_count;
                }
            }
            else
                _count = 0;
        }

    private:
        unsigned* _count;
};

template <typename T>
class ExternalAtomicRefCounted
{
    public:
        int refs() const
        { return rc ? atomicGet(*rc) : 0; }

    protected:
        ExternalAtomicRefCounted()
        : rc(0)
        { }

        bool unlink(T* object)
        {
            if (object && atomicDecrement(*rc) <= 0)
            {
                delete rc;
                // no need to set rc to 0 since the pointer is either
                // destroyed or another object is linked in
                return true;
            }
            else
                return false;
        }

        void link(const ExternalAtomicRefCounted& ptr, T* object)
        {
            if (object)
            {
                if (ptr.rc == 0)
                    rc = new atomic_t(1);
                else
                {
                    rc = ptr.rc;
                    atomicIncrement(*rc);
                }
            }
            else
                rc = 0;
        }

    private:
        volatile atomic_t* rc;
};

/**
    \param T The managed object type
*/
template <typename T>
/** \brief deleter policy for smart pointer

    The DeletePolicy implements the method, which instructs the SmartPtr to free the
    object which it helds by deleting it.
*/
class DeletePolicy
{
    protected:
        static void destroy(T* ptr)
        { delete ptr; }
};

template <typename T>
class FreeDestroyPolicy
{
    protected:
        void destroy(T* ptr)
        { free(ptr); }
};

template <typename ObjectType>
class ArrayDestroyPolicy
{
    protected:
        static void destroy(ObjectType* ptr)
        { delete[] ptr; }
};


// auxiliary type to enable copies and assignments (now global)
template<typename T>
struct AutoPtrRef {
    T* ptr;
    AutoPtrRef(T* rhs)
    : ptr(rhs)
    { }
};


template<typename T,
          typename Destroy = DeletePolicy<T> >
/** @brief Policy based Auto pointer.

    The DestroyPolicy implements the method for destroying the object once
    the auto pointer runs out of scope. By default, the object is destroyed
    by deleting it, but this can be overridden by implementing a different
    DestroyPolicy. The DestroyPolicy needs to implement a method
    destroy(T*), which releases the underlying pointer.

    \param T Contained type.
    \param DestroyPolicy policy, to destroy the object.
*/
class AutoPtr : public Destroy
{
    private:
        T* ap;    // refers to the actual owned object (if any)

    public:
        typedef T element_type;

        // constructor
        explicit AutoPtr(T* ptr = 0)
        : ap(ptr)
        { }

        // copy constructors (with implicit conversion)
        // - note: nonconstant parameter
        AutoPtr (AutoPtr& rhs)
          : ap(rhs.release()) {
        }

        template<class Y>
        AutoPtr (AutoPtr<Y>& rhs)
          : ap(rhs.release()) {
        }

        // assignments (with implicit conversion)
        // - note: nonconstant parameter
        AutoPtr& operator= (AutoPtr& rhs) {
            reset(rhs.release());
            return *this;
        }

        template<class Y>
        AutoPtr& operator= (AutoPtr<Y>& rhs)
        {
            reset(rhs.release());
            return *this;
        }

        // destructor
        ~AutoPtr()
        { this->destroy(ap); }

        // value access
        T* get() const {
            return ap;
        }
        T& operator*() const {
            return *ap;
        }
        T* operator->() const {
            return ap;
        }
        bool operator!() const {
            return ap == 0;
        }
        /** @brief Bool conversion operator

            An AutoPtr can be implicitly converted to bool. True is returned
            when the raw pointer is not null, false if it is null

            @return false if the raw pointer is null.
        */
        operator bool () const
        { return ap != 0; }

        // release ownership
        T* release() {
            T* tmp(ap);
            ap = 0;
            return tmp;
        }

        // reset value
        void reset (T* ptr=0) {
            if (ap != ptr) {
                this->destroy(ap);
                ap = ptr;
            }
        }

        /* special conversions with auxiliary type to enable copies and assignments
          */
        AutoPtr(AutoPtrRef<T> rhs) throw()
          : ap(rhs.ptr) {
        }
        AutoPtr& operator= (AutoPtrRef<T> rhs) throw() {  // new
              reset(rhs.ptr);
              return *this;
        }
        template<class Y>
        operator AutoPtrRef<Y>() throw() {
            return AutoPtrRef<Y>(release());
        }
        template<class Y>
        operator AutoPtr<Y>() throw() {
            return AutoPtr<Y>(release());
        }
};

/** \brief Equality comparison operator.
*/
template <typename T, typename D, typename T2, typename D2>
bool operator==(const AutoPtr<T, D>& a, const AutoPtr<T2, D2>& b)
{ return a.get() == b.get(); }


template <typename T, typename D, typename T2>
bool operator==(const AutoPtr<T, D>& a, const T2* b)
{ return a.get() == b; }


/** \brief Equality comparison operator.
*/
template <typename T, typename D, typename T2, typename D2>
bool operator!=(const AutoPtr<T, D>& a, const AutoPtr<T2, D2>& b)
{ return a.get() != b.get(); }


template <typename T, typename D, typename T2>
bool operator!=(const AutoPtr<T, D>& a, const T2* b)
{ return a.get() != b; }


/** \brief Less-than comparison operator
*/
template <typename T, typename D, typename T2, typename D2>
bool operator<(const AutoPtr<T, D>& a, const AutoPtr<T2, D2>& b)
{ return a.v() < b.get(); }


template <typename T, typename D, typename T2>
bool operator<(const AutoPtr<T, D>& a, const T2* b)
{ return a.get() < b; }




template <typename T,
          typename OwnershipPolicy = ExternalRefCounted<T>,
          typename DestroyPolicy = DeletePolicy<T> >
/** @brief Policy based smart pointer.

    The SmartPtr implements a model that determines how the contained
    raw pointer is managed. The default model is RefCounted, which uses a
    non-intrusive reference counting mechanism.
    A model-policy needs to implement two functions called link() and unlink() to
    manage a raw pointer.
    The DestroyPolicy implements the method for destroying the object once
    the smart pointer detects, that the object needs to be freed. By default
    the object is destroyed by deleting it, but this can be overridden by
    implementing a different DestroyPolicy. The DestroyPolicy needs to
    implement a method destroy(T*), which releases the underlying pointer.

    \param T Contained type.
    \param Model Model for linking/unlinking.
    \param DestroyPolicy policy, to destroy the object.
*/
class SmartPtr : public OwnershipPolicy,
                 public DestroyPolicy
{
    private:
        //! \brief The raw pointer
        T* object;

    public:
        /** \brief Default Constructor.

            The contained pointer is set to null in the default constructor.
        */
        SmartPtr()
        : object(0)
        {}

        /** \brief Constructs from a pointer to manage.

            The behaviour depends on the Model. When the default model is
            used both SmartPtr will manage the heap object.

            \param ptr The raw pointer
        */
        SmartPtr(T* ptr)
        : object(ptr)
        { this->link(*this, ptr); }

        /** \brief Copy constructor.

            The behaviour depends on the Model. When the default model is
            used both SmartPtr will reference the same heap object.

            \param ptr The other SmartPtr
        */
        SmartPtr(const SmartPtr& ptr)
        : object(ptr.object)
        { this->link(ptr, ptr.object); }

        /** \brief Destructor.

            The behaviour depends on the Model. When the default model is
            used the managed raw pointer will be deleted when the last
            SmartPtr goes out of scope.
        */
        ~SmartPtr()
        { 
            if (this->unlink(object))
                this->destroy(object); 
        }

        /** \brief Assign from another SmartPtr.

            The behaviour depends on the Model. When the default model is
            used both SmartPtr will reference the same heap object. Nothing
            will happen on self assignment.

            \param ptr The other SmartPtr
            \return self reference
        */
        SmartPtr& operator=(const SmartPtr& ptr)
        {
            if(object == ptr.object) {
                return *this;
            }

            if (this->unlink(object))
                this->destroy(object);

            object = ptr.object;

            this->link(ptr, object);

            return *this;
        }

        /** \brief Returns a pointer to the heap object

            Return a copy of the pointer that this object owns. The
            SmartPtr still manages the memory.

            \return Pointer to the heap object
        */
        T* operator->() const { return object; }

        /** \brief Returns a reference to the heap object

            If this SmartPtr is null this function will crash.

            \return Reference to the heap object
        */
        T& operator*() const
        { return *object; }

        /** \brief Negate operator

            This operator matches the behavious of raw pointers. True will be
            returned if the raw pointer is null.

            \return true if the raw pointer is null.
        */
        bool operator! () const
        { return object == 0; }

        /** \brief Bool conversion operator

            A SmartPtr can be implicitly converted to bool. True is returned
            when the raw pointer is not null, false if it is null

            \return false if the raw pointer is null.
        */
        operator bool () const
        { return object != 0; }

        /** \brief Returns a pointer to the heap object

            Return a copy of the pointer that this object owns. The
            SmartPtr still manages the memory.

            \return Pointer to the heap object
        */
        T* get()
        { return object; }

        /** \brief Returns a pointer to the heap object

            Return a copy of the pointer that this object owns. The
            SmartPtr still manages the memory.

            \return Pointer to the heap object
        */
        const T* get() const
        { return object; }
};


/** \brief Equality comparison operator

    @related SmartPtr
*/
template <typename T, class O, typename D, typename T2, typename O2, typename D2>
bool operator==(const SmartPtr<T, O, D>& a, const SmartPtr<T2, O2, D2>& b)
{ return a.get() == b.get(); }


template <typename T, typename O, typename D, typename T2>
bool operator==(const SmartPtr<T, O, D>& a, const T2* b)
{ return a.get() == b; }


/** \brief Equality comparison operator.

    @related SmartPtr
*/
template <typename T, typename O, typename D, typename T2, typename O2, typename D2>
bool operator!=(const SmartPtr<T, O, D>& a, const SmartPtr<T2, O2, D2>& b)
{ return a.get() != b.get(); }


template <typename T, typename O, typename D, typename T2>
bool operator!=(const SmartPtr<T, O, D>& a, const T2* b)
{ return a.get() != b; }


/** \brief Less-than comparison operator

    @related SmartPtr
*/
template <typename T, typename O, typename D, typename T2, typename O2, typename D2>
bool operator<(const SmartPtr<T, O, D>& a, const SmartPtr<T2, O2, D2>& b)
{ return a.get() < b.get(); }


template <typename T, typename O, typename D, typename T2>
bool operator<(const SmartPtr<T, O, D>& a, const T2* b)
{ return a.get() < b; }


template <typename T, typename M, typename D >
void fixup(const Pt::FixupInfo& fixup, SmartPtr<T, M, D>& fixme)
{
    if( fixup.isNull() )
    {
        fixme = SmartPtr<T,M, D>();
    }
    else
    {
        const SmartPtr<T,M, D>* to = fixup.getTarget< SmartPtr<T, M, D> >();
        fixme = *to;
    }
}


template <typename T, typename M, typename D >
void load(const LoadInfo& li, SmartPtr<T, M, D>& sp)
{
    if( li.in().isReference() )
    {
        li.in().loadReference(sp);
    }
    else
    {
        li.load(sp);
    }
}


template <typename T, typename M>
void operator >>=(const Pt::SerializationInfo& si, SmartPtr<T, M>& sp)
{
    sp = new T();
    si >>= *sp;
}


template <typename T, typename M, typename D >
void save(Pt::SaveInfo& si, const SmartPtr<T, M, D>& sp)
{
    if( ! sp.get() || ! si.save( *sp ) )
    {
        si.out() <<= sp.get();
    }
}


template <typename T, typename M, typename D >
void operator <<=(Pt::SerializationInfo& si, const SmartPtr<T, M, D>& sp)
{
    if( sp.getPointer() )
    {
        si <<= *sp;
    }
    else
    {
        si <<= sp.getPointer();
    }
}

} // namespace Pt

#endif // PT_SMARTPTR_H
