/* Copyright (C) 2008 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_API_TYPETRAITS_H
#define PT_API_TYPETRAITS_H

#include <Pt/Api.h>
#include <cstddef>

/** @addtogroup Pt-TypeTraits

    @brief Compile-time type traits and runtime type information.

    Two types are useful to get type information: Pt::TypeTraits and
    Pt::TypeInfo. TypeTraits are used for generic programming, for example
    to deduce the pointer type in templated code, or to branch differently
    for const and non-const types. The TypeInfo class is a wrapper for
    std::type_info, which makes it easier to store and compare type
    information. The std::type_info is normally not copyable and
    comparable. Pt::SourceInfo is used to store information about a
    location in the source code.
*/

namespace Pt {

/** @brief Traits for type properties.

    The %TypeTraits can determine at compile time whether a type is const,
    is a reference or is a pointer. This is useful for template programming,
    where code can branch differently depending on the type traits.

    @headerfile TypeTraits.h <Pt/TypeTraits.h>
    @ingroup Pt-TypeTraits
*/
template <typename T>
struct TypeTraits
{
    //! @brief The derived value type.
    typedef IMPLEMENTATION_DEFINED Value;

    //! @brief The derived const qualified value type.
    typedef IMPLEMENTATION_DEFINED ConstValue;

    //! @brief The derived reference type.
    typedef IMPLEMENTATION_DEFINED Reference;

    //! @brief The derived const qualified reference type.
    typedef IMPLEMENTATION_DEFINED ConstReference;

    //! @brief The derived pointer type.
    typedef IMPLEMENTATION_DEFINED Pointer;

    //! @brief The derived const qualified reference type.
    typedef IMPLEMENTATION_DEFINED ConstPointer;

    //! @brief If the type is const 1, otherwise 0.
    static const unsigned int isConst;

    //! @brief If the type is volatile 1, otherwise 0.
    static const unsigned int isVolatile;

    //! @brief If the type is a pointer 1, otherwise 0.
    static const unsigned int isPointer;

    //! @brief If the type is a reference 1, otherwise 0.
    static const unsigned int isReference;
};

} // namespace Pt

#endif
