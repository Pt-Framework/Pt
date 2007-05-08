/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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
#ifndef Pt_TypeInfo_h
#define Pt_TypeInfo_h

#include <Pt/Api.h>
#include <Pt/IfElse.h>
#include <Pt/TypeTraits.h>
#include <string>


namespace Pt {

    template <typename T>
    struct TypeInfoBase {
        typedef T Value;
        typedef const T ConstType;
        typedef T& Reference;
        typedef const T& ConstReference;
        typedef T* Pointer;
        typedef const T* ConstPointer;
    };


    /** @brief CTTI for for non-const value types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo : public TypeInfoBase<T> {
        static const char* typeName()
        { return TypeTraits<T>::typeName(); }
    };


    /** @brief CTTI for for const value types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<const T> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T>
    const std::string TypeInfo<const T>::_typeName = "const " + std::string( TypeTraits<T>::typeName() );


    /** @brief CTTI for for non-const reference types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<T&> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T>
    const std::string TypeInfo<T&>::_typeName = std::string( TypeTraits<T>::typeName() ) + "&";


    /** @brief CTTI for for const reference types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<const T&> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T>
    const std::string TypeInfo<const T&>::_typeName = "const " + std::string( TypeTraits<T>::typeName() ) + "&";


    /** @brief CTTI for for non-const pointer types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<T*> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T>
    const std::string TypeInfo<T*>::_typeName = std::string( TypeTraits<T>::typeName() ) + "*";


    /** @brief CTTI for for const pointer types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<const T*> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T>
    const std::string TypeInfo<const T*>::_typeName = "const " + std::string( TypeTraits<T>::typeName() ) + "*";


    /** @brief CTTI for for fixed-size array types
        @ingroup CTTI
    */
    template <typename T, std::size_t N>
    struct TypeInfo<T[N]> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T, std::size_t N>
    const std::string TypeInfo<T[N]>::_typeName = std::string( TypeTraits<T>::typeName() ) + "[N]";


    /** @brief CTTI for for array types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<T[]> : public TypeInfoBase<T> {
        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename T>
    const std::string TypeInfo<T[]>::_typeName = std::string( TypeTraits<T>::typeName() ) + "[]";


    /** @brief CTTI for for member function types
        @ingroup CTTI
    */
    template <typename R, class C>
    struct TypeInfo< R C::* > {
        typedef R ReturnT;
        typedef C ClassT;
        typedef void Arg1T;
        typedef void Arg2T;
        typedef void Arg3T;

        static const char* typeName()
        { return _typeName.c_str(); }

        static const std::string _typeName;
    };

    template <typename R, class C>
    const std::string TypeInfo< R C::* >::_typeName = std::string( TypeTraits<R>::typeName() ) + " " +
                                                  std::string( TypeTraits<C>::typeName() ) + "::*";


    /** @brief CTTI for for void
        @ingroup CTTI
    */
    template <>
    struct PT_API TypeInfo<void> {
        typedef void Value;
        typedef void ConstType;
        typedef void Reference;
        typedef void ConstReference;
        typedef void* Pointer;
        typedef void* ConstPointer;

        static const char* typeName()
        { return TypeTraits<void>::typeName(); }
    };


    struct PT_API isTrue {
        operator bool() const
        { return true; }

        static const unsigned value = 1;
    };


    struct PT_API isFalse {
        operator bool() const
        { return false; }

        static const unsigned value = 0;
    };


    /** @brief  CTTI predicate for type comparson
        @ingroup CTTI
    */
    template <typename A, typename B>
    struct isEqual : public isFalse {
    };


    template <typename T>
    struct isEqual<T, T> : public isTrue {
    };


    /** @brief  CTTI predicate for array types
        @ingroup CTTI
    */
    template <typename T>
    struct isArray : public isFalse {
    };


    template <typename T, std::size_t N>
    struct isArray<T[N]> : public isTrue {
    };


    template <typename T>
    struct isArray<T[]> : public isTrue {
    };


    template <typename T>
    struct isArray<T**> : public isTrue {
    };


    //! CTTI predicate for reference types
    template <typename T>
    struct isReference : public isFalse {
    };


    template <typename T>
    struct isReference<T&> : public isTrue {
    };


    //! CTTI predicate for pointer types
    template <typename T>
    struct isPointer : public isFalse {
    };


    template <typename T>
    struct isPointer<T*> : public isTrue {
    };


    template <typename T, std::size_t N>
    struct isPointer<T[N]> : public isTrue {
    };


    template <typename T>
    struct isPointer<T[]> : public isTrue {
    };


    //! CTTI predicate for const types
    template <typename T>
    struct isConst : public isFalse {
    };


    template <typename T>
    struct isConst<const T> : public isTrue {
    };


    template <typename T>
    struct isConst<const T*> : public isTrue {
    };


    template <typename T>
    struct isConst<const T&> : public isTrue {
    };


    //! internal implementation of isClass<T>
    struct isClassImpl {
        template <typename C>
        static char test(...)
        { return char(); }


        template <typename C>
        static long test(int C::*)
        { return long(); }
    };


    //! CTTI predicate for class/struct types
    template <typename T,
              class Base = typename IfElse<sizeof( isClassImpl::test<T>(0) ) != 1, isTrue, isFalse>::ResultT >
    struct isClass : public Base {
    };


    //! CTTI predicate for value types
    template <typename T,
              class Base = typename IfElse<sizeof( isClassImpl::test<T>(0) ) == 1, isTrue, isFalse>::ResultT >
    struct isValue : public Base {
    };

} // !namespace Pt


#endif
