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
#ifndef Pt_TypeTraits_h
#define Pt_TypeTraits_h

#include <Pt/Api.h>
#include <string>
#include <typeinfo>


namespace Pt {

    /** @brief Traits struct for types used in the CTTI system
        @ingroup Pt

        Compile time type information (CTTI) is implemented in Pt by the two
        classes TypeTraits and TypeInfo. A number of specialisations allows
        compile type branching in gerneric code depending on the type.
        TypeTraits can be used on its own or as part of TypeInfo, which can
        give more detailed information about a type. All types to be used
        in the CTTI system can be added non-intrusively, by specialising the
        TypeTraits.
    */
    template <typename T>
    struct TypeTraits
    {
        /** @brief Indicate that TypeTraits  exist

            This function returns false in the generic version of TypeTraits
            meaning that no specialisation for a type exists. All specialised
            TypeTraits shall return true.
        */
        static bool isSpecialized()
        { return false; }

        /** @brief Get type name as string

            The generic version, which is used if no specialisation exists returns
            the string "unknown". All specialised TypeTraits shall return a fully
            qualified typename.
        */
        static const char* typeName()
        { return "unknown"; }
    };


    /** @brief Type traits for void
        @ingroup Pt

        This is the type information for void used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<void>
    {
         /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "void"; }
    };


    /** @brief Type traits for bool
        @ingroup Pt

        This is the type information for bool used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<bool>
    {
        /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "bool"; }
    };


    /** @brief Type traits for char
        @ingroup Pt

        This is the type information for char used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<char>
    {
        /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "char"; }
    };


    /** @brief Type traits for int
        @ingroup Pt

        This is the type information for int used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<int>
    {
        /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "int"; }
    };


    /** @brief Type traits for float
        @ingroup Pt

        This is the type information for float used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<float>
    {
        /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "float"; }
    };


    /** @brief Type traits for double
        @ingroup Pt

        This is the type information for double used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<double>
    {
        /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "double"; }
    };


    /** @brief Type traits for void
        @ingroup Pt

        This is the type information for std::string used by the CTTI system
        in Pt. It specialises the conceptional TypeTraits struct.
    */
    template <>
    struct PT_API TypeTraits<std::string>
    {
        /** @brief Indicate that TypeTraits exist

            Specialisation of TypeTraits<>
        */
        static bool isSpecialized()
        { return true; }

        /** @brief Get type name as string

            Specialisation of TypeTraits<>
        */
        static const char* typeName()
        { return "std::string"; }
    };

} // !namespace Pt


#endif
