/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#ifndef PT_SYSTEM_SHAREDLIB_H
#define PT_SYSTEM_SHAREDLIB_H

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/File.h>
#include <Pt/System/Directory.h>

#include <string>


namespace Pt {

namespace System {


/**
 * @brief Shared library loader
 *
 * This class can be used to dynamically load shared libraries and
 * resolve symbols from it. The example below shows how to retrieve
 * the address of the function 'myProcedure' in library 'MySharedLib':
 *
 *
 * @code
 *     typedef int (*MyProcType)();
 *     SharedLib shlib(Pt::System::File("MySharedLib.dll"));
 *     void* procAddr = shlib["myProcedure"];
 *     MyProcType proc;
 *     int result = -1;
 *
 *     proc = (MyProcType)procAddr;
 *     result = proc();
 * @endcode
 */
class PT_SYSTEM_API SharedLib : private NonCopyable {
    public:

        /**
         * @brief Default Constructor which does not load a library.
         */
        SharedLib();

        /**
         * @brief Constructs the object and loads the shared library specified in the File argument.
         *
         * The given File object has to specify the file by its platform-specific name, including
         * any library prefix and suffix. The path to the file (including its file name) is passed
         * to the platform's loadLibrary-function as is.
         *
         * The library is loaded immediately.
         * 
         * @param libraryFile A File object referencing the library in the file system.
         */
        SharedLib(const File& libraryFile);

        /**
         * @brief Constructs the object and loads the shared library specified from the base directory
         * and platform-independent library name.
         *
         * A platform-specific library location is constructed from the given directory and library name.
         * The library is searched for in the given base directory. The library name is extended by the
         * platform's library prefix and the platform's default library extension. (There is no check done
         * for any existing prefixes or extensions before the prefix and extension is added.) The library
         * name thus needs to be the plain library name.
         * 
         * The library is loaded in this constructor immediately.
         * 
         * @param baseDirectory The directory in which the references library resides.
         * @param libraryName The base library name (without any platform-specific information) which
         * is extended with the platform specific prefix and extension.
         */
        SharedLib(const Directory& baseDirectory, const std::string& libraryName);

        /**
         * @brief Destructor
         * The destructor unloads the shared library from memory.
         */
        ~SharedLib();

        /**
         * @brief Loads the given shared library.
         *
         * The given File object has to specify the file by its platform-specific name, including
         * any library prefix and suffix. The path to the file (including its file name) is passed
         * to the platform's loadLibrary-function as is.
         *
         * If a library was previously loaded by calling the constructor with a library file
         * or by calling one of the open()-methods, this method fails and throws a SystemError.
         *
         * @param libraryFile A File object referencing the library in the file system.
         * @throws SystemError If a library already was opened for this SharedLib object.
         */
        SharedLib& open(const File& libraryFile);

        /**
         * @brief Loads the given shared library specified by the base directory and platform-
         * independent library name.
         *
         * A platform-specific library location is constructed from the given directory and library name.
         * The library is searched for in the given base directory. The library name is extended by the
         * platform's library prefix and the platform's default library extension. (There is no check done
         * for any existing prefixes or extensions before the prefix and extension is added.) The library
         * name thus needs to be the plain library name.
         * 
         * If a library was previously loaded by calling the constructor with a library file
         * or by calling one of the open()-methods, this method fails and throws a SystemError.
         *
         * @param baseDirectory The directory in which the references library resides.
         * @param libraryName The base library name (without any platform-specific information) which
         * is extended with the platform specific prefix and extension.
         * @throws SystemError If a library already was opened for this SharedLib object.
         */
        SharedLib& open(const Directory& baseDirectory, const std::string& libraryName);

        /**
         * @brief Resolve symbol from shared library.
         *
         * @param symbol the name of the symbol to be resolved.
         * @return the address of the symbol or NULL if it was not found
         */
        void* operator[](const char* symbol);

        /**
         * @brief Resolves a symbol from the shared library.
         *
         * @param symbol the name of the symbol to be resolved.
         * @return the address of the symbol or NULL if it was not found
         */
        void* resolve(const char* symbol);

        /**
         * @brief Object status checking.
         *
         * @return NULL if object is in a failed state, otherwise non-NULL
         */
        operator void*();

        /**
         * @brief Object status checking.
         *
         * @return true if object is in a failed state
         */
        bool operator!();
        
        /**
         * @brief Returns a File object pointing to this library file in the file system.
         *
         * @return The File object pointing to this library file in the file system.
         */
        const Pt::System::File& libraryFile() const;

    public:
        /**
         * @brief Loads and resolves a symbol from the shared library.
         *
         * The given File object has to specify the file by its platform-specific name, including
         * any library prefix and suffix. The path to the file (including its file name) is passed
         * to the platform's loadLibrary-function as is.
         *
         * @param libraryFile A File object referencing the library in the file system.
         * @param symbol The name of the symbol to be resolved.
         * @return The address of the symbol or NULL if it was not found
         */
        static void* openResolve(const File& libraryFile, const char* symbol);

        /**
         * @brief Loads and resolves a symbol from the shared library.
         *
         * A platform-specific library location is constructed from the given directory and library name.
         * The library is searched for in the given base directory. The library name is extended by the
         * platform's library prefix and the platform's default library extension. (There is no check done
         * for any existing prefixes or extensions before the prefix and extension is added.) The library
         * name thus needs to be the plain library name.
         *
         * @param baseDirectory The directory in which the references library resides.
         * @param libraryName The base library name (without any platform-specific information) which
         * is extended with the platform specific prefix and extension.
         * @param symbol The name of the symbol to be resolved.
         * @return The address of the symbol or NULL if it was not found
         */
        static void* openResolve(const Directory& directory, const std::string& libraryName, const char* symbol);

        /**
         * @brief Converts the given File object of a library to a platform specific representation
         * of this library File object.
         *
         * The path of the file object is extended by the platform's default library prefix and
         * the platform's library extension. The extension only is added if the file does not yet
         * have an extension; otherwise the extensions is kept. The parent path is not changed.
         *
         * Example: Using Linux the file "/users/ab/library" is converted to "/users/ab/liblibrary.so".
         *
         * @param file The File object, referencing a library, which is converted to a platform-
         * specific library representation of this file.
         * @return The platform-specific library representation of this file.
         */
        static File convertToPlatformSpecificLibraryFile(const File& file);

        /**
         * @brief Creates a File object from the given base directory and platform-independent library name.
         *
         * A platform-specific library location is constructed from the given directory and library name.
         * The library name is extended by the platform's library prefix and the platform's default library
         * extension. (There is no check done for any existing prefixes or extensions before the prefix and
         * extension is added.) The library name thus needs to be the plain library name.
         *
         * A new File object containing said information is returned.
         *
         * @param baseDirectory The directory in which the references library resides.
         * @param libraryName The base library name (without any platform-specific information) which
         * is extended with the platform specific prefix and extension.
         * @return The platform-specific library file.
         */
        static File createLibraryFile(const Pt::System::Directory& baseDir, const std::string& libraryName);

    private:
        class SharedLibImpl* _impl;
        
        File _libraryFile;
};

} // namespace System

} // namespace Pt

extern "C" {

    /// \exclude
    PT_SYSTEM_API void pt_system_testSharedLib();

}

#endif
