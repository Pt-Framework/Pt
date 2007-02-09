/***************************************************************************
 *   Copyright (C) 2006 by Tommi Mäkitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Duerner                               *
 *   Copyright (C) 2006 by Stefan Bueder                                    *
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
#ifndef PT_SMARTPTR_H
#define PT_SMARTPTR_H

#include <Pt/Types.h>
#include <cstdio>

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
            //! \brief Unlink a smart pointer from a managed object
            void unlink(T* object)
            {
                if (object)
                {
                    if (next == this)
                    {
                        delete object;
                        object = 0;
                    }
                    else
                    {
                        next->prev = prev;
                        prev->next = next;
                    }
                    next = prev = this;
                }
            }

            //! \brief lLnk a smart pointer to a managed object
            void link(const RefLinked& ptr, T* object)
            {
                prev = &ptr;
                next = ptr.next;
                prev->next = this;
                next->prev = this;
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
            void unlink(T* object)
            {
                if (object)
                    object->release();
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
            size_t refs() const
            { return *_count; }

        protected:
            ExternalRefCounted()
            : _count(0)
            { }

            //! \brief unlink a smart pointer from a managed object
            void unlink(T* object)
            {
                if ( _count && !--*_count)
                {
                    delete _count;
                    delete object;
                }
            }

            //! \brief link a smart pointer to a managed object
            void link(const ExternalRefCounted& ptr, T* object)
            {
                if(ptr._count == 0) {
                    _count = new size_t(1);
                }
                else
                {
                    _count = ptr._count;
                    ++*_count;
                }
            }

        private:
            size_t* _count;
    };


    /** \param T Contained type
        \param Model Model for linking/unlinking
    */
    template <typename T,
              typename Model = ExternalRefCounted<T> >
    /** \brief Policy based smart pointer.
        The SmartPtr implements a model that determines how the contained
        raw pointer is managed. The default model is RefCounted, which uses a
        non-intrusive reference counting mechanism.
        A policy needs to implement two functions called link() and unlink() to
        manage a raw pointer.
    */
    class SmartPtr : public Model
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

            /** \brief Copy contructor.

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

                \param ptr The other SmartPtr
            */
            ~SmartPtr()
            { this->unlink(object); }

            /** \brief Assign from another SmartPtr.

                The behaviour depends on the Model. When the default model is
                used both SmartPtr will reference the same heap object. Nothing
                will happen on self assignment.

                \param ptr The other SmartPtr
                \return self reference
            */
            SmartPtr& operator=(const SmartPtr& ptr)
            {
                if(this == &ptr) {
                    return *this;
                }
                this->unlink(object);
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

            /** \brief Equality comparison operator

                Two SmartPtr are considered equal when they contain the same
                heap object.

                \return true if the SmartPtr point to the same object.
            */
            bool operator== (const T* p) const
            { return object == p; }

            /** \brief Equality comparison operator

                Two SmartPtr are considered equal when they contain the same
                heap object.

                \return true if the SmartPtr point to different objects.
            */
            bool operator!= (const T* p) const
            { return object != p; }

            /** \brief Less-than comparison operator

                A SmartPtr is considered less than another when the adress of
                the raw pointer is less.

                \return true if the raw pointer is less than the other raw pointer.
            */
            bool operator< (const T* p) const
            { return object < p; }

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
            T* getPointer()
            { return object; }

            /** \brief Returns a pointer to the heap object

                Return a copy of the pointer that this object owns. The
                SmartPtr still manages the memory.

                \return Pointer to the heap object
            */
            const T* getPointer() const
            { return object; }

            /** \brief Implicit conversion to the contained pointer type.

                Return a copy of the pointer that this object owns. The
                SmartPtr still manages the memory.
            */

            operator T* ()
            { return object; }

            /** \brief Implicit conversion to the contained pointer type.

                Return a copy of the pointer that this object owns. The
                SmartPtr still manages the memory.
            */
            operator const T* () const
            { return object; }
    };

} // namespace Pt

#endif // PT_SMARTPTR_H
