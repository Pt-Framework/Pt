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

class PathImpl;

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

        // TODO: explicit? Path p = "x"; not allowed?

        /** @brief Construct from a path name.
        */
        explicit Path(const Pt::String& s);

        /** @brief Construct from an UTF-8 encoded path name.
        */
        explicit Path(const char* s);

        /** @brief Construct from an UTF-8 encoded path name.
        */
        explicit Path(const char* s, std::size_t n);

        /** @brief Destructor.
        */
        ~Path();

        // Assignments

        /** @brief Assigns a path name.
        */
        Path& assign(const Path& p);

        /** @brief Assigns a path name.
        */
        Path& assign(const Pt::String& s);

        /** @brief Assigns an UTF-8 encoded path name.
        */
        Path& assign(const char* s);

        /** @brief Assigns an UTF-8 encoded path name.
        */
        Path& assign(const char* s, std::size_t n);

        /** @brief Assignment operator.
        */
        Path& operator=(const Path& p)
        { return assign(p); }

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
        Path& append(const char* s);

        /** @brief Appends an UTF-8 encoded path name.
        */
        Path& append(const char* s, std::size_t n);

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
        Path& concat(const Path& p);

        /** @brief Concatenates a path name.
        */
        Path& concat(const Pt::String& s);

        /** @brief Concatenates a path name.
        */
        Path& concat(const char* s);

        /** @brief Appends an UTF-8 encoded path name.
        */
        Path& concat(const char* s, std::size_t n);

        /** @brief Concatenates a path name.
        */
        Path& operator+=(const Path& p)
        { return concat(p); }
        
        /** @brief Concatenates a path name.
        */
        Path& operator+=(const Pt::String& s)
        { return concat(s); }

        /** @brief Concatenates a path name.
        */
        Path& operator+=(const char* s)
        { return concat(s); }

        // Modifiers

        void clear();

        // Query

        bool empty() const;

        // TODO: return Path objects
        Pt::String fileName() const;

        Pt::String dirName() const;

        Pt::String baseName() const;

        Pt::String extension() const;

        // Comparison

        int compare(const Path& p) const;

        // Conversion

        /** @brief Returns the path name.
        */
        Pt::String toString() const;

        /** @brief Returns the path name in local encoding.
        */
        std::string toLocal() const;

        // Others

        static Pt::String dirsep();

        static Pt::String curdir();

        static Pt::String updir();

        // Implementation

        const PathImpl* impl() const
        { return _impl; }

        PathImpl* impl()
        { return _impl; }

    private:
        PathImpl* _impl;
        std::string _pathData;
};

/** @brief Compares two paths.

    @related Path
*/
inline Path operator/(const Path& a, const Path& b)
{ 
    return Path(a) /= b;
}

inline Path operator/(const Path& a, const Pt::String& b)
{ 
    return Path(a) /= b;
}

inline Path operator/(const Path& a, const char* b)
{ 
    return Path(a) /= b;
}

/** @brief Compares two paths.

    @related Path
*/
inline bool operator==(const Path& a, const Path& b)
{ 
    return a.compare(b) == 0; 
}

/** @brief Compares two paths.

    @related Path
*/
inline bool operator!=(const Path& a, const Path& b)
{ 
    return a.compare(b) != 0; 
}

/** @brief Compares two paths.

    @related Path
*/
inline bool operator<(const Path& a, const Path& b)
{ 
    return a.compare(b) < 0;
}

} // namespace System

} // namespace Pt

#endif // Pt_System_Path_h
