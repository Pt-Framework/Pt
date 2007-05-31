/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 by PTV AG                                          *
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
#if !defined(PT_Environment_H)
#define PT_Environment_H

#include "Pt/System/Api.h"
#include "Pt/System/Directory.h"
#include <string>

namespace Pt {

namespace System {

    //! @brief Environment class.
    /**
    */

    class PT_SYSTEM_API Environment
    {
        private:
            class EnvironmentImpl* _impl;

        protected:
            Environment();
            ~Environment();

        public:
            /**
             *  @brief Returns the extension for a shared library on the current system.
             *  E.g. ".so" on Linux, ".dll" on Windows             
             */
            static const std::string& sharedLibraryExtension();

            /**
             *  @brief Returns the prefix for a shared library on the current system.
             *  E.g. "lib" on Linux, "" on Windows             
             */
            static const std::string& sharedLibraryPrefix();

            /**
             *  @brief Returns the path separator on the current system.
             *  E.g. "/" on Linux, "\" on Windows             
             */
            static char pathSeparator();

            /**
             *  @brief Returns the system path on the current system.
             *  E.g. "/" (root) on Linux, "c:\" on Windows             
             */
            static Directory systemDirectory();

            /**
             *  @brief Returns the current working directory.
             */
            static Directory currentDirectory();

            /**
             *  @brief Returns the systems tmp directory.
             *               
             *  On Windows, the environment variables TEMP and TMP are checked.
             *  On Linux, the environment variable TMPDIR is checked and if not set, 
             *  "/tmp" is returned.
             *  If none of the environment variables are set and the default system 
             *  tmp directory does not exist, the current directory is returned.                                       
             */
            static Directory tempDirectory();
            
            /*
             * @return the total physical memory in kiloByte (kB)
             */
            static unsigned long getTotalMemory();
            /*
             * @return the available physical memory in kiloByte (kB)
             */
            static unsigned long getFreeMemory();

            /*
             * @return the amount of memory used by the calling process in kiloByte (kB)
             */
            static unsigned long getProcessMemoryUsage();
            

    };

} // !namespace system

} // !namespace Pt

#endif // PT_Environment_H



