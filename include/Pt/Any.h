#ifndef Pt_Any_h
#define Pt_Any_h

#include <Pt/Api.h>
#include <Pt/AnyTraits.h>
#include <Pt/TypeInfo.h>

#include <iostream>
#include <map>


namespace Pt {

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
            explicit Any(const T& val);

            /** @brief Copy constructor

                Constructs the Any by copying the value of the other Any. It
                is legal to assign an empty Any. If an exception is thrown
                during construction, the Any will be empty and the exception
                is porpagated.

                @param val Any to assign
            */
            Any(const Any& val);

            /** @brief Destructor

                Deallocates the memory needed to hold the value.
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

            template <typename T>
            struct Bind {
                Bind( const std::string& typeName = TypeTraits<T>::typeName() )
                { Any::bind<T>( typeName ); }
            };

        protected:
            template <typename T>
            static void bind(const std::string& typeName)
            { Any::initMap().insert( std::make_pair<std::string, void (Any::*)()>(typeName, &Any::init<T>) ); }

            static std::map<std::string, void (Any::*)()>& initMap();

        private:
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

}


#endif


