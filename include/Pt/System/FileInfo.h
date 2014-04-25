/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef Pt_System_FileInfo_h
#define Pt_System_FileInfo_h

#include <Pt/System/Api.h>
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace System {

/** @brief Provides information about a node in the file-system.

    The Pt::System::FileInfo class provides operations to query information
    about files and directories in the file system and to add, remove and
    modify them. %FileInfo objects can be created with a path, are assignable,
    comparable and can be used as keys for e.g. std::map. The path must not
    be valid to successfully construct a %FileInfo object, but it can be
    checked whether a file exists and what type of file it is, as shown in
    the following example:

    @code
    Pt::System::FileInfo fi("/tmp/logout.txt");
    
    if( fi.type() == Pt::System::FileInfo::File )
    {
        std::cout << fi.path() << ": " << fi.size() << " bytes." << std::endl;
    }
    else
    {
        std::cout << fi.path() << " is invalid." << std::endl;
    }
    @endcode
    
    All operations are also available as non-member functions, so it
    is not neccessary to create temporary %FileInfo objects. Only the paths
    to files or directories are required to perform file system operations.
    The next example illustrates some of the non-member functions for file
    operations:

    @code
    try
    {
        // create a temporary file
        Pt::System::FileInfo::createFile("/tmp/tmpfile1");

        // move it to a new location
        Pt::System::FileInfo::move("/tmp/tmpfile1", "/tmp/tmpfile2");

        // remove the file
        Pt::System::FileInfo::remove("/tmp/tmpfile2");
    }
    catch(const Pt::System::AccessFailed& e)
    {
        std::cerr << "file error: " << e.resource() << std::endl;
    }
    @endcode

    The code shown above creates a file, moves it to a new location and
    finally deletes it. If an operation fails, for example because the file
    could not be created, an exception of type %AccessFailed is thrown. This
    is also the case for all other operations such as size(), createFile(),
    createDirectory(), resize() and remove(). The exception reports the name
    of the resource that could not be accessed.

    @ingroup FileSystem
*/
class PT_SYSTEM_API FileInfo
{
    public:
        //! @brief File-node type
        enum Type
        {
            Invalid = 0,   //!< Invalid file type
            Directory = 1, //!< Directory
            File = 2       //!< Regular file
        };

    public:
        //! @brief Default constructor
        FileInfo()
        {}

        /** @brief Constructs from the path.
        */
        explicit FileInfo(const std::string& path);

        /** @brief Constructs from the path.
        */
        explicit FileInfo(const char* path);

        //! @brief Destructor
        ~FileInfo()
        {}

        //! @brief Clears the state.
        void clear();

        /** @brief Returns the full path of node in the file-system

            This method may return a relative path, or a fully qualified one
            depending on how this object was constructed.
        */
        const std::string& path() const
        { return _path; }

        //! @brief Returns the file size.
        Pt::uint64_t size() const
        { return FileInfo::size(_path); }

        //! @brief Returns the file type.
        Type type() const
        { return FileInfo::type(_path); }

        //! @brief Returns the name including an exension
        std::string name() const
        { return FileInfo::name(_path); }

        /** @brief Returns the parent directory path

            This method might return an empty string if the node was created
            without a complete path. If the directory is located in the root
            directory of a unix file system, a slash ("/") is returned. A
            returned directory path always ends with a trailing path separator
            character. (A backslash in Windows and a slash in Unix, for example.)
        */
        std::string dirName() const
        { return FileInfo::dirName(_path); }

        //! @brief Returns the file name without the exension
        std::string baseName()
        { return FileInfo::baseName(_path); }

    public:
        //! @brief Returns the type of file at the \a path
        static Type type(const std::string& path);

        //! @brief Returns the type of file at the \a path
        static Type type(const char* path);

        //! @brief Returns the size of the file in bytes
        static Pt::uint64_t size(const std::string& path);

        //! @brief Returns the size of the file in bytes
        static Pt::uint64_t size(const char* path);

        //! @brief Returns true if a file or directory exists at the \a path
        static bool exists(const std::string& path)
        { return type(path) != Invalid; }

        //! @brief Returns true if a file or directory exists at the \a path
        static bool exists(const char* path)
        { return type(path) != Invalid; }

        //! @brief Returns the name including an exension
        static std::string name(const std::string& path);

        //! @brief Returns the name including an exension
        static std::string name(const char* path);

        //! @brief Returns the parent directory path
        static std::string dirName(const std::string& path);

        //! @brief Returns the parent directory path
        static std::string dirName(const char* path);

        //! @brief Returns the file name without the exension
        static std::string baseName(const std::string& path);

        //! @brief Returns the file name without the exension
        static std::string baseName(const char* path);

        //! @brief Returns the file name extension or an empty string if not present
        static std::string extension(const std::string& path);

        //! @brief Returns the file name extension or an empty string if not present
        static std::string extension(const char* path);

        //! @brief Creates a new file.
        static void createFile(const std::string& path);

        //! @brief Creates a new file.
        static void createFile(const char* path);

        //! @brief Creates a new directory.
        static void createDirectory(const std::string& path);

        //! @brief Creates a new directory.
        static void createDirectory(const char* path);

        //! @brief Resizes a file.
        static void resize(const std::string& path, Pt::uint64_t n);

        //! @brief Resizes a file.
        static void resize(const char* path, Pt::uint64_t n);

        //! @brief Removes a file or directory.
        static void remove(const std::string& path);

        //! @brief Removes a file or directory.
        static void remove(const char* path);

        //! @brief Moves a file or directory.
        static void move(const std::string& path, const std::string& to);

        //! @brief Moves a file or directory.
        static void move(const char* path, const char* to);

    public:
        //! @brief Returns the string representing the separator in path names
        static const char* sep();

        //! @brief Returns the string representing the current directory in path names
        static const char* curdir();

        //! @brief Returns the string representing the parent directory in path names
        static const char* updir();

    public:
        //! @internal
        std::string& path()
        { return _path; }
    
    private:
        std::string _path;
};


/** @brief Compare two %FileInfo objects.

    @related FileInfo
*/
inline bool operator<(const FileInfo& a, const FileInfo& b)
{
    return a.path() < b.path();
}

/** @brief Compare two %FileInfo objects.

    @related FileInfo
*/
inline bool operator==(const FileInfo& a, const FileInfo& b)
{
    return a.path() == b.path();
}

/** @brief Compare two %FileInfo objects.

    @related FileInfo
*/
inline bool operator!=(const FileInfo& a, const FileInfo& b)
{
    return !(a == b);
}

} // namespace System

} // namespace Pt

#endif // Pt_System_FileInfo_h
