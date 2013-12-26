/*
 * Copyright (C) 2004-2011 by Marc Boris Duerner
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

#ifndef Pt_Any_h
#define Pt_Any_h

#include <Pt/TypeTraits.h>
#include <typeinfo>
#include <cstring>
#include <new>

namespace Pt {

/** @brief Contains an arbitrary type.

    %Any can contain any other type that is default- and copy constructible.
    When a value is assigned to an %Any a copy is made, just like when a type
    is inserted in a standard C++ container. The contained type can be accessed
    via Pt::any_cast<>. It is only possible to get the contained value if the
    type matches.

    @code
    Any a = 5;
    int i = any_cast<int>( a );    // i is 5 now
    float f = any_cast<float>( a ) // throws std::bad_cast
    @endcode

    @ingroup CoreTypes
*/
class Any
{
    template <typename T>
    friend T any_cast(const Any&);

    template <typename T>
    friend struct AnyCast;

    public:
        /** @internal */
        class Value
        {
            public:
                virtual ~Value() {}
                virtual Value* clone(char*) const = 0;
                virtual const std::type_info& type() const = 0;
                virtual bool isRef() const = 0;
                virtual void* get() = 0;
                virtual const void* get() const = 0;
        };

        /** @internal */
        template <typename T>
        class BasicValue : public Value
        {
            public:
                BasicValue(const T& value = T())
                : _value(value)
                { }

                const T& value() const
                { return _value;}

                T& value()
                { return _value;}

                virtual const std::type_info& type() const
                { return typeid(T); }

                virtual Value* clone(char* data) const
                { 
                    return sizeof(BasicValue<T>) > Any::sizeofData ? new BasicValue(_value)
                                                                   : new(data) BasicValue(_value); 
                }

                virtual bool isRef() const
                { return false; }

                virtual void* get()
                { return &_value; }

                virtual const void* get() const
                { return &_value; }

            private:
                T _value;
        };

        /** @internal */
        template <typename T>
        class BasicRefValue : public Value
        {
            public:
                BasicRefValue(T* value)
                : _value(value)
                { }

                virtual const std::type_info& type() const
                { return typeid(T); }

                virtual Value* clone(char* data) const
                { return new(data) BasicRefValue(_value); }

                virtual bool isRef() const
                { return true; }

                virtual void* get()
                { return (void*) _value; }

                virtual const void* get() const
                { return _value; }

            private:
                T* _value;
        };

        /** @internal */
        class RefValue : public Value
        {
            public:
                RefValue(void* value, const std::type_info& ti)
                : _value(value)
                , _ti(&ti)
                { }

                virtual bool isRef() const
                { return true; }

                virtual const std::type_info& type() const
                { return *_ti; }

                virtual Any::Value* clone(char* data) const
                { return new(data) RefValue(_value, *_ti); }

                virtual void* get()
                { return _value; }

                virtual const void* get() const
                { return _value; }

            private:
                void* _value;
                const std::type_info* _ti;
        };

        //! @internal
        bool dataUsed() const
        { return static_cast<const void*>(_value) == static_cast<const void*>(_data); }

    public:
        /** @brief Construct with value

            Constructs the %Any from an value of arbitrary type. The type
            to be assigned must be copy-constructible. Memory is allocated
            to store the value. If an exception is thrown during
            construction, the Any will be empty and the exception is
            porpagated.

            @param type Value to assign
        */
        template <typename T>
        Any(const T& type)
        : _value(0)
        {
            _value = sizeof(BasicValue<T>) > Any::sizeofData ? new BasicValue<T>(type)
                                                             : new(static_cast<void*>(_data)) BasicValue<T>(type);
        }

        /** @brief Construct with reference

            Constructs the %Any from a pointer to an arbitrary type. The
            constructed %Any will not make a copy, but only keep a shallow
            pointer. It is the resposibility of the caller to make sure
            the type pointed to exists longer than the created %Any.

            @param type Value to assign
        */
        template <typename T>
        explicit Any(T* type)
        : _value(0)
        {
            // storage is always large enough for BasicRefValue
            _value = new(static_cast<void*>(_data)) BasicRefValue<T>(type);
        }

        explicit Any(void* type, const std::type_info& ti)
        : _value(0)
        {
            // storage is always large enough for RefValue
            _value = new(static_cast<void*>(_data)) RefValue(type, ti);
        }

        /** @brief Default constructor

            Constructs an empty any. No memory needs to be allocated for
            empty Anys.
        */
        Any()
        : _value(0)
        { }

        /** @internal @brief Assigns an abstract value
        */
        Any& assign(Value* value);

        /** @brief Copy constructor

            Constructs the Any by copying the value of the other Any. It
            is legal to assign an empty Any. If an exception is thrown
            during construction, the Any will be empty and the exception
            is porpagated.

            @param val Any to assign
        */
        Any(const Any& val);

        /** @brief Destructor

            Deallocates the memory needed to hold the value. This will
            also destruct the contained type.
        */
        ~Any()
        {
            if (_value)
            {
                if (dataUsed())
                    _value->~Value();
                else
                    delete _value;
            }
        }

        /** @brief Clear content

            Removes the stored type resulting in a destructor call
            for the stored type. All memory required to hold the value
            is deallocated.
        */
        void clear()
        {
            if (_value)
            {
                if (dataUsed())
                    _value->~Value();
                else
                    delete _value;
                _value = 0;
            }
        }

        /** @brief Check if empty

            Returns true if no value has been assigned, false otherwise.

            @return True if empty
        */
        inline bool empty() const
        { return !_value; }

        /** @brief Swap values

            The member function swaps the assigned values between *this and right.
            No exceptions are thrown, and no memory needs to be allocated.

            @param other Other any to swap value
            @return self reference
        */
        Any& swap(Any& other);

        inline bool isRef() const
        { return _value && _value->isRef(); }

        /** @brief Returns type info of assigned type

            Returns the std::type_info of the currently assigned type. If the
            Any is empty the type_info of void is returned.

            @return Type info
        */
        const std::type_info& type() const
        { return _value ? _value->type() : typeid(void); }

        /** @brief Assign value

            Assigns a value of an arbitrary type. The type to be assigned
            must be copy-constructible. Memory is allocated to store the value.
            If an exception is thrown during construction, the Any will remain
            unaltered and the exception is porpagated.

            @param rhs Value to assign
        */
        template <typename T>
        Any& operator=(const T& rhs)
        {
            clear();
            _value = sizeof(BasicValue<T>) > Any::sizeofData ? new BasicValue<T>(rhs)
                                                             : new(static_cast<void*>(_data)) BasicValue<T>(rhs);
            return *this;
        }

        /** @brief Assign reference.

            Initializes an %Any from a pointer to an arbitrary type. The
            assignment will not make a copy, but only keep a shallow
            pointer. It is the resposibility of the caller to make sure
            the type pointed to exists longer than the %Any.

            @param rhs Value to assign
        */
        template <typename T>
        Any& operator=(T* rhs)
        {
            clear();
            _value = new(static_cast<void*>(_data)) BasicRefValue<T>(rhs);
            return *this;
        }

        /** @brief Assign value of other Any

            Assignes the value of another Any by copying the value of the
            other Any. It is legal to assign an empty Any. If an exception
            is thrown during assignment, the Any will remain unchanged and
            the exception is porpagated.

            @param rhs Any to assign
        */
        Any& operator=(const Any& rhs);

        /** @internal @brief Returns the value which the %Any contains
        */
        const Any::Value* value() const
        { return _value; }

        /** @internal @brief Returns the value which the %Any contains
        */
        Any::Value* value()
        { return _value; }

        /** @brief Get pointer to stored value

            Returns a pointer to the stored value or 0 if the %Any is
            empty. Use Any::type to find out which type is stored in
            the &Any.

            @return Pointer to stored value or 0 if empty
        */
        void* get()
        {
            if(_value)
                return _value->get();

            return 0;
        }

        /** @brief Get pointer to stored value

            Returns a pointer to the stored value or 0 if the %Any is
            empty. Use Any::type to find out which type is stored in
            the &Any.

            @return Pointer to stored value or 0 if empty
        */
        const void* get() const
        {
            if(_value)
                return _value->get();

            return 0;
        }

    private:
        //! @internal Size of the internal storage for small types.
        static const unsigned sizeofData = sizeof(RefValue);

        /** @internal */
        Value* _value;

        //! @internal Storage for small types.
        char _data[sizeofData];
};

/** @internal Implementation of any_cast.
*/
template <typename T>
struct AnyCast
{
    static T cast(const Any& any)
    {
        // NOTE:
        // - the first if(...) may not work properly on Linux when loading libs,
        //   so there is also a comparison of string names (second if(...))
        // - but: the name() method necessary for string comparison does not
        //   exist on WinCE, so the second if(...) is not compiled for WinCE
        typedef typename TypeTraits<T>::Value ValueT;

        if( any.type() == typeid(ValueT) )
        {
            void* v = any._value->get();
            ValueT* vtp = reinterpret_cast<ValueT*>(v);
            return *vtp;
        }

#ifndef _WIN32_WCE
        else if( 0 == std::strcmp(any.type().name(), typeid(ValueT).name() ) )
        {
            void* v = any._value->get();
            ValueT* vtp = reinterpret_cast<ValueT*>(v);
            return *vtp;
        }
#endif

        throw std::bad_cast();
    }
};

/** @internal Implementation of any_cast.
*/
template <typename T>
struct AnyCast<T*>
{
    static T* cast(const Any& any)
    {
        // NOTE:
        // - the first if(...) may not work properly on Linux when loading libs,
        //   so there is also a comparison of string names (second if(...))
        // - but: the name() method necessary for string comparison does not
        //   exist on WinCE, so the second if(...) is not compiled for WinCE
        typedef typename TypeTraits<T>::Value ValueT;

        if( any.type() == typeid(ValueT) )
        {
            void* v = any._value->get();
            ValueT* vtp = reinterpret_cast<ValueT*>(v);
            return vtp;
        }

#ifndef _WIN32_WCE
        else if( 0 == std::strcmp(any.type().name(), typeid(ValueT).name() ) )
        {
            void* v = any._value->get();
            ValueT* vtp = reinterpret_cast<ValueT*>(v);
            return vtp;
        }
#endif

        throw std::bad_cast();
    }
};

/** @brief Get contained value

    This function is used to get the contained value from an %Any. It is
    not possible to get a float out of an %Any if the contained value is
    an int, but the typeid's must match. It is, however, possible to
    get a const reference to the contained type.

    @param any Any to read to
    @return contained value
    @throw std::bad_cast on type mismatch
*/
template <typename T>
inline T any_cast(const Any& any)
{
    return AnyCast<T>::cast(any);
}


inline Any& Any::assign(Value* value)
{
    clear();
    _value = value->clone(_data);
    return *this;
}


inline Any::Any(const Any& val)
: _value(0)
{
    if (val._value)
        _value = val._value->clone(_data);
}


inline Any& Any::swap(Any& rhs)
{
    if (dataUsed())
    {
        if (rhs.dataUsed())
        {
            Any tmp(*this);
            *this = rhs;
            rhs = tmp;
        }
        else
        {
            Value* tmp = _value;
            _value = rhs._value;
            rhs._value = tmp->clone(rhs._data);
            tmp->~Value();
        }
    }
    else
    {
        if (rhs.dataUsed())
        {
            Value* tmp = rhs._value;
            rhs._value = _value;
            _value = tmp->clone(_data);
            tmp->~Value();
        }
        else
        {
            Value* tmp = rhs._value;
            rhs._value = _value;
            _value = tmp;
        }
    }

    return *this;
}


inline Any& Any::operator=(const Any& rhs)
{
    clear();

    if (rhs._value)
        _value = rhs._value->clone(_data);

    return *this;
}

} // namespace xxx

#endif
