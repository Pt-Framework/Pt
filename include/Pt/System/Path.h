/*
 * Copyright (C) 2006-2014 Marc Boris Duerner
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

#ifndef Pt_System_Path_h
#define Pt_System_Path_h

#include <Pt/System/Api.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace System {

/** @brief Represents a path in the file-system.
*/
class PT_SYSTEM_API Path
{
    public:
        /** @brief Default constructor.
        */
        Path();

        /** @brief Copy constructor.
        */
        Path(const Path& p);

        /** @brief Construct from a path name.
        */
        explicit Path(const Pt::String& s);

        /** @brief Construct from an UTF-8 encoded path name.
        */
        explicit Path(const char* s);

        /** @brief Destructor.
        */
        ~Path();

        // Assignments

        /** @brief Assignment operator.
        */
        Path& operator=(const Path& p);

        /** @brief Assigns a path name.
        */
        Path& assign(const Pt::String& s);

        /** @brief Assigns an UTF-8 encoded path name.
        */
        Path& assign(const char* s);

        /** @brief Assigns a path name.
        */
        Path& operator=(const Pt::String& s)
        { return assign(s); }

        /** @brief Assigns an UTF-8 encoded path name.
        */
        Path& operator=(const char* s)
        { return assign(s); }

        // Appends

        /** @brief Appends a path name.
        */
        Path& append(const Path& p);

        /** @brief Appends a path name.
        */
        Path& append(const Pt::String& s);

        /** @brief Appends an UTF-8 encoded path name.
        */
        Path& append(const char* from);

        /** @brief Appends an UTF-8 encoded path name.
        */
        Path& append(const char* from, std::size_t size);

        /** @brief Appends a path name.
        */
        Path& operator/=(const Path& p)
        { return append(p); }

        /** @brief Appends a path name.
        */
        Path& operator/=(const Pt::String& s)
        { return append(s); }

        /** @brief Appends an UTF-8 encoded path name.
        */
        Path& operator/=(const char* s)
        { return append(s); }

        // Concatenation

        /** @brief Concatenates a path name.
        */
        Path& concat(const Pt::String& s);

        /** @brief Concatenates a path name.
        */
        Path& operator+=(const Pt::String& s)
        { return concat(s); }
        
        // Modifiers

        void clear();
        
        // Conversion

        Pt::String toString() const;

        /** @brief Returns the path name in local encoding.
        */
        std::string toLocal() const;

    private:
        class PathImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_Path_h
