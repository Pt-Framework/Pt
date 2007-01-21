/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_Any_h
#define Pt_Any_h

#include <Pt/Api.h>
#include <Pt/AnyTraits.h>
#include <Pt/TypeInfo.h>

#include <iostream>
#include <map>


namespace Pt {

    /** Contains any type

        Any can contain any other type that is default- and copy constructible
        and less-than and equality comparable. The behaviour of types used in
        Anys can be refined by specialising AnyTraits for the type. When a
        value is assigne to an Any a copy is made, just like when a type is
        inserted in a standard C++ container. The contained type can be
        accessed via Pt::any_cast<>. It is only possible to get the contained
        value if the type matches

        @code
            Any a = 5;
            int i = any_cast<int>( a );    // i is 5 now
            float f = any_cast<float>( a ) // throws std::bad_cast
        @endcode

        Anys can be compared by the contained types and values. Two Anys are
        considered equal when the contained values are equal and of the same
        type. A special case is less-than comparison, when the contained
        types are different. std::type_info::before will be used to decide
        which Any is less.

        @code
            Any a = 6;
            Any b = 6;
            Any c = '6';
            Any d = 1;

            // true, same type, same value
            a == b;

            // false, different types
            b == c;

            // true, same type and less
            d \< a;

            // implementation dependent
            d \< c;
        @endcode

        Any supports named initialisation from a typename string or a type.
        Before this feature can be used all required types must be bound to
        typenames. This can be done conveniently with Any::Bind objects. By
        default the C++ built-in types are bound already, as well as
        std::string and the types of the Pt framework.

        @code
            Any::Bind bindMyType<MyType>("MyType");

            // An empty Any
            Any a;

            // Any will contain a default constructed int
            a.init<int>();

            // Any will contain a default constructed MyType
            a.init("MyType");

            // any_cast will return the contained MyType
            MyType mt = any_cast<MyType>( a ); 
        @endcode

        Anys can be written to streams easily, however reading from a stream
        into an Any requires initialisation if the Any. If the Any is not
        initialised to a type nothing will be read. If the Any is initalised
        the contained type will be tried to read from the stream.

        @code
            std::istringstream ss("5");
            Any a;

            // This reads nothing
            ss >> i;

            // Set Any to hold an int
            a.init<int>();

            // Now read from the stream
            ss >> a;
        @endcode
    */
    class PT_API Any
    {
        template <typename T>
        friend T any_cast(const Any&);

        public:
            /** @internal
            */
            class Value
            {
                public:
                    virtual ~Value() {}
                    virtual Value* clone() const = 0;
                    virtual const char* typeName() const = 0;
                    virtual const std::type_info& type() const = 0;
                    virtual void output(std::ostream& os) const = 0;
                    virtual void input(std::istream& os) = 0;
                    virtual bool equal(const Value& value) const = 0;
                    virtual bool lt(const Value& value) const = 0;
            };

            /** @internal
            */
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

                    virtual const char* typeName() const
                    { return TypeTraits<T>::typeName(); }

                    virtual const std::type_info& type() const
                    { return typeid(T); }

                    virtual Any::Value* clone() const
                    { return new BasicValue(_value); }

                    virtual void output(std::ostream& os) const
                    { AnyTraits<T>::output(os, _value); }

                    virtual void input(std::istream& is)
                    { AnyTraits<T>::input(is, _value); }

                    virtual bool equal(const Value& value) const
                    {
                        try {
                            const BasicValue<T>& bv = dynamic_cast< const BasicValue<T>& >(value);
                            return (bv._value == this->_value);
                        }
                        catch(...) {}

                        return false;
                    }

                    virtual bool lt(const Value& value) const
                    {
                        try {
                            const BasicValue<T>& bv = dynamic_cast< const BasicValue<T>& >(value);
                            return (bv._value < this->_value);
                        }
                        catch(...) {}

                        bool x = !( typeid(T).before( value.type() ) );
                        return !x;
                    }

                private:
                    T _value;
            };

            /** @brief Default constructor

                Constructs an empty any. No memory needs to be allocated for
                empty Anys.
            */
            Any();

            /** @brief Construct with value

                Constructs the Any from an value of arbitrary type. The type
                to be assigned must be copy-constructible. Memory is allocated
                to store the value. If an exception is thrown during
                construction, the Any will be empty and the exception is
                porpagated.

                @param val Value to assign
            */
            template <typename T>
            Any(const T& val);

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
            ~Any();

            /** @brief Clear content

                Removes the stored type resulting in a destructor call
                for the stored type. All memory required to hold the value
                is deallocated.
            */
            void clear();

            /** @brief Check if empty

                Returns true if no value has been assigned, false otherwise.

                @return True if empty
            */
            inline bool empty() const
            { return !_value; }

            /** @brief Init by type

                Initializes the Any to hold a default constructed type. If
                an exception is thrown, the Any remains unaltered and the
                exception is propagated.
            */
            template <typename T>
            void init();

            /** @brief Init by typename

                Initializes the Any to hold a default constructed type which
                has been previously registered under a typename using
                Any::Bind. If the typename is not found or an exception is
                thrown the Any remains unaltered and the exception is
                propagated.

                TODO: should this throw on unknown typenames?

                @typeName the registered typename
            */
            void init(const std::string& typeName);

            /** @brief Swap values

                The member function swaps the assigned values between *this and right.
                No exceptions are thrown, and no memory needs to be allocated.

                @param other Other any to swap value
                @return self reference
            */
            Any& swap(Any& other);

            /** @brief Check typename of assigned type

                Returns the typename of the currently assigned type. If the
                Any is empty "void" is returned.

                @return Typename
            */
            const char* typeName() const
            { return _value ? _value->typeName() : "void"; }

            /** @brief Returns type info of assigned type

                Returns the std::type_info of the currently assigned type. If the
                Any is empty the type_info of void is returned.

                @return Type info
            */
            const std::type_info& type() const
            { return _value ? _value->type() : typeid(void); }

            /** @brief Write value to stream

                This member function writes the stored value to a std::ostream
                as defined in the AnyTraits for the stored type.

                @param os Output stream
            */
            void output(std::ostream& os) const;

            /** @brief Read value from stream

                This member function reads a value from the stream and stores
                it in the Any. The same type that is currently assigned to the
                Any will be read from the stream and its value assigned to the
                Any. If the Any is empty nothing is read.

                @param is Input stream
            */
            void input(std::istream& is);

            /** @brief Assign value

                Assigns a value of an arbitrary type. The type to be assigned
                must be copy-constructible. Memory is allocated to store the value.
                If an exception is thrown during construction, the Any will remain
                unaltered and the exception is porpagated.

                @param val Value to assign
            */
            template <typename T>
            Any& operator=(const T& rhs);

            /** @brief Assign value of other Any

                Assignes the value of another Any by copying the value of the
                other Any. It is legal to assign an empty Any. If an exception
                is thrown during assignment, the Any will remain unchanged and
                the exception is porpagated.

                @param val Any to assign
            */
            Any& operator=(const Any& rhs);

            /** @brief Check if equal

                Returns true if the contained type and the passed type are
                equal and have equal values.

                @return True if equal
            */
            template <typename T>
            bool operator==(const T& a) const;

            /** @brief Check if equal

                Returns true if the contained types are equal and have
                equal values.

                @return True if equal
            */
            bool operator==(const Any& a) const;

            /** @brief Check if inequal

                Returns true if the contained types have different values
                or if the conatained types are different.

                @return True if different
            */
            bool operator!=(const Any& a) const;

            /** @brief Check if less

                Returns true if the value of the contained type is less than
                the contained value of the other Any. If the contained types
                are different type_info::before decides which Any is less.

                @return True if less
            */
            bool operator<(const Any& a) const;

            /** @brief Type binder for types used in Any

                If you want to use the named type initialisation of Any,
                you can bind your types to typenames. Your types need to
                be bound to a name before Any::init(const std::string&) is
                going to work.

                /sa Any
            */
            template <typename T>
            struct Bind
            {
                /** @brief Binds a type to a name

                    With the use of Any::Bind objects you can bind your types
                    at static initialisation time. By default the typename
                    will be taken from the TypeTraits of the to be bound type.
                */
                Bind( const std::string& typeName = TypeTraits<T>::typeName() )
                { Any::bind<T>( typeName ); }
            };

        protected:
            /** @brief Binds a type to a name

                With this function you can bind your types to be used under
                a typename  by Any. This function is not thread-safe, do not
                call it from threads other than the main thread. a better
                approach would be to register your types with Any::Bind.
            */
            template <typename T>
            static void bind(const std::string& typeName)
            { Any::initMap().insert( std::make_pair<std::string, void (Any::*)()>(typeName, &Any::init<T>) ); }

        private:
            /** @internal */
            static std::map<std::string, void (Any::*)()>& initMap();

            /** @internal */
            Value* _value;
    };

    /** @brief Read value from stream

        @sa Any::output

        @param is Output stream
        @param val Any to write
        @return Output stream reference
    */
    PT_API std::ostream& operator<<(std::ostream& os, const Pt::Any& val);

    /** @brief Read value from stream

        @sa Any::input

        @param is Input stream
        @param val Any to read to
        @return Input stream reference
    */
    PT_API std::istream& operator>>(std::istream& is, Pt::Any& val);

    /** @brief Get contained value

        This function is used to get the contained value from an Any. It is
        not possible to get a float out of an Any if the contained value is
        an int, but the typeid's must match. It is, however, possible to
        get a const reference ton the contained type.

        @param any
        @param val Any to read to
        @return contained value
        @throw std::bad_cast on type mismatch
    */
    template <typename T>
    inline T any_cast(const Any& any)
    {
        typedef typename Pt::TypeInfo<T>::Value ValueT;

        if( any.type() == typeid(ValueT) ) {
            const Any::BasicValue<ValueT>* value;
            value = static_cast< const Any::BasicValue<ValueT>* >(any._value);
            return value->value();
        }

        throw std::bad_cast();
    }

}


namespace Pt {

    template <typename T>
    Any::Any(const T& type)
    : _value(0)
    { (*this) = type; }


    template <typename T>
    Any& Any::operator=(const T& rhs)
    {
        Any::Value* tmp = new BasicValue<T>(rhs);
        delete _value;
        _value = tmp;
        return *this;
    }


    template <typename T>
    void Any::init()
    {
        Any::Value* tmp = new BasicValue<T>;
        delete _value;
        _value = tmp;
    }


    template <typename T>
    bool Any::operator==(const T& value) const
    {
        return _value->equal( BasicValue<T>(value) );
    }

}


#endif


