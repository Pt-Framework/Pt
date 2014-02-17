#ifndef Pt_TypeTraits_h
#define Pt_TypeTraits_h

#include <Pt/Api.h>
#include <cstddef>

namespace Pt {

/** @brief Traits for type properties.

    The %TypeTraits can determine at compile time whether a type is const,
    is a reference or is a pointer. This is useful for template programming,
    where code can branch differently depending on the type traits.

    @ingroup Utilities
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

    //! @brief If the type is a pointer 1, otherwise 0.
    static const unsigned int isPointer;

    //! @brief If the type is a reference 1, otherwise 0.
    static const unsigned int isReference;
};

} // namespace Pt

#endif // Pt_TypeTraits_h
