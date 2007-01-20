#ifndef Pt_Any_h
#define Pt_Any_h

#include <Pt/AnyTraits.h>
#include <Pt/TypeInfo.h>
#include <Pt/Singleton.h>
#include <Pt/Clonable.h>
#include <iostream>
#include <map>


namespace Pt {

	class  PT_API Any {
		template <typename T>
		friend T any_cast(const Any&);

		public:
			class Value : public Pt::Clonable<Value> {
				public:
					virtual ~Value() {}
					virtual const char* typeName() const = 0;
					virtual const std::type_info& type() const = 0;
					virtual void output(std::ostream& os) const = 0;
					virtual void input(std::istream& os) = 0;
					virtual bool equal(const Value& value) const = 0;
					virtual bool lt(const Value& value) const = 0;
			};

			template <typename T>
			class BasicValue : public Value {
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

		public:
			template <typename T>
			explicit Any(const T& type);

			Any();

			Any(const Any& val);

			~Any();

			void clear();

			inline bool empty() const
			{ return !_value; }

			template <typename T>
			void init();

			void init(const std::string& typeName);

			Any& swap(Any& rhs);

			const char* typeName() const
			{ return _value ? _value->typeName() : "void"; }

			const std::type_info& type() const
			{ return _value ? _value->type() : typeid(void); }

			void output(std::ostream& os) const;

			void input(std::istream& is);

		public:
			template <typename T>
			Any& operator=(const T& rhs);

			Any& operator=(const Any& rhs);

			// class member to prevent implicit lhs conversion
			bool operator==(const Any& a) const;

			// class member to prevent implicit lhs conversion
			bool operator!=(const Any& a) const;

			// class member to prevent implicit lhs conversion
			bool operator<(const Any& a) const;

		public:
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


	PT_API std::ostream& operator<<(std::ostream& os, const Pt::Any& val);


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
		delete _value;
		_value = new BasicValue<T>(rhs);
		return *this;
	}

	template <typename T>
	void Any::init()
	{
		delete _value;
		_value = new BasicValue<T>;
	}

}


#endif


