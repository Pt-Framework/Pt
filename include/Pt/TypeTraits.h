/*
  Copyright (C) 2016 Marc Boris Duerner
  Copyright (C) 2016 Aloysius Indrayanto

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef Pt_TypeTraits_h
#define Pt_TypeTraits_h

#include <Pt/Api.h>
#include <cstddef>

namespace Pt {

template <typename T, T V>
struct IntegralConstant
{
    static const T value = V;
    typedef T ValueType;
    typedef IntegralConstant<T, V> Type;

    operator ValueType() const
    { return value; }
};


template <bool V>
using BoolConstant = IntegralConstant<bool, V>;

typedef BoolConstant<false> FalseType;
typedef BoolConstant<true> TrueType;


template<class T, class U>
struct IsSame : FalseType
{};


template<class T>
struct IsSame<T, T> : TrueType
{};


template <bool Condition, typename TrueT, typename FalseT>
struct IfElse
{
    typedef TrueT Type;
};


template <typename TrueT, typename FalseT>
struct IfElse<false, TrueT, FalseT>
{
    typedef FalseT Type;
};


template <std::size_t Index, typename T, typename... Ts>
struct NthType
{
    typedef typename NthType<Index - 1, Ts...>::type type;
};


template <typename T, typename... Ts>
struct NthType<0, T, Ts...>
{
    typedef T type;
};


template <typename T>
struct TypeTraitsBase
{
    typedef T Value;
    typedef const T ConstValue;
    typedef T& Reference;
    typedef const T& ConstReference;
    typedef T* Pointer;
    typedef const T* ConstPointer;
};

//! @cond PT_DOXYGEN_HIDDEN

template <typename T>
struct TypeTraits : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 0;
};

//! @endcond

template <typename T>
struct TypeTraits<const T> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 0;
};

template <typename T>
struct TypeTraits<volatile T> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 0;
};

template <typename T>
struct TypeTraits<const volatile T> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<T&> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 1;
};


template <typename T>
struct TypeTraits<const T&> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 1;
};


template <typename T>
struct TypeTraits<volatile T&> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 1;
};


template <typename T>
struct TypeTraits<const volatile T&> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 1;
};


template <typename T>
struct TypeTraits<T*> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<const T*> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<volatile T*> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<const volatile T*> : public TypeTraitsBase<T>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T, std::size_t N>
struct TypeTraits<T[N]> : public TypeTraitsBase<T*>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T, std::size_t N>
struct TypeTraits<const T[N]> : public TypeTraitsBase<const T*>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T, std::size_t N>
struct TypeTraits<volatile T[N]> : public TypeTraitsBase<volatile T*>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T, std::size_t N>
struct TypeTraits<const volatile T[N]> : public TypeTraitsBase<const volatile T*>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<T[]> : public TypeTraitsBase<T*>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<const T[]> : public TypeTraitsBase<const T*>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<volatile T[]> : public TypeTraitsBase<volatile T*>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename T>
struct TypeTraits<const volatile T[]> : public TypeTraitsBase<const volatile T*>
{
    static const unsigned int isConst = 1;
    static const unsigned int isVolatile = 1;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename R, typename... Args>
struct TypeTraits<R(*)(Args...)> : public TypeTraitsBase<R(*)(Args...)>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 1;
    static const unsigned int isReference = 0;
};


template <typename R, typename... Args>
struct TypeTraits<R(Args...)> : public TypeTraitsBase<R(*)(Args...)>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 0;
};


template <typename R, typename... Args>
struct TypeTraits<R(&)(Args...)> : public TypeTraitsBase<R(*)(Args...)>
{
    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 1;
};


template <>
struct TypeTraits<void>
{
    typedef void Value;
    typedef void ConstType;
    typedef void Reference;
    typedef void ConstReference;
    typedef void* Pointer;
    typedef void* ConstPointer;

    static const unsigned int isConst = 0;
    static const unsigned int isVolatile = 0;
    static const unsigned int isPointer = 0;
    static const unsigned int isReference = 0;
};


template <typename Base, typename Derived>
class IsCompatibleImpl
{
    private:
        struct YesType
        {
            char value;
        };

        struct NoType
        {
            char value[2];
        };

        typedef typename TypeTraits<Base>::Value BaseType;
        typedef typename TypeTraits<Derived>::Value DerivedType;

        static YesType test(BaseType*);
        static NoType test(...);

    public:
        enum
        {
            value = sizeof(test(static_cast<DerivedType*>(0))) == sizeof(YesType)
        };
};


template <typename Base, typename Derived>
class IsCompatible
: public BoolConstant< IsCompatibleImpl<Base, Derived>::value >
{};


template <typename T>
struct IntTraits
{};

template <>
struct IntTraits<signed char>
{
    typedef unsigned char Unsigned;
    typedef signed char Signed;

    static const unsigned int isSigned = 1;
};

template <>
struct IntTraits<unsigned char>
{
    typedef unsigned char Unsigned;
    typedef signed char Signed;

    static const unsigned int isSigned = 0;
};

template <>
struct IntTraits<short>
{
    typedef unsigned short Unsigned;
    typedef signed short Signed;

    static const unsigned int isSigned = 1;
};

template <>
struct IntTraits<unsigned short>
{
    typedef unsigned short Unsigned;
    typedef signed short Signed;

    static const unsigned int isSigned = 0;
};

template <>
struct IntTraits<int>
{
    typedef unsigned int Unsigned;
    typedef signed int Signed;

    static const unsigned int isSigned = 1;
};

template <>
struct IntTraits<unsigned int>
{
    typedef unsigned int Unsigned;
    typedef signed int Signed;

    static const unsigned int isSigned = 0;
};

template <>
struct IntTraits<long>
{
    typedef unsigned long Unsigned;
    typedef signed long Signed;

    static const unsigned int isSigned = 1;
};

template <>
struct IntTraits<unsigned long>
{
    typedef unsigned long Unsigned;
    typedef signed long Signed;

    static const unsigned int isSigned = 0;
};

template <>
struct IntTraits<long long>
{
    typedef unsigned long long Unsigned;
    typedef signed long long Signed;

    static const unsigned int isSigned = 1;
};

template <>
struct IntTraits<unsigned long long>
{
    typedef unsigned long long Unsigned;
    typedef signed long long Signed;

    static const unsigned int isSigned = 0;
};

} // namespace Pt

#endif // Pt_TypeTraits_h
